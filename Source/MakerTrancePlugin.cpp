#include "DistrhoPlugin.hpp"
#include "MakerTranceShared.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <vector>

START_NAMESPACE_DISTRHO

namespace
{
using namespace MakerTrance;

inline float midiNoteToHz(const int note) noexcept
{
    return 440.0f * std::pow(2.0f, (static_cast<float>(note) - 69.0f) / 12.0f);
}

inline float fastSaw(const float phase) noexcept { return phase * 2.0f - 1.0f; }
inline float fastSquare(const float phase) noexcept { return phase < 0.5f ? 1.0f : -1.0f; }
inline float fastTriangle(const float phase) noexcept { return 1.0f - 4.0f * std::abs(phase - 0.5f); }

inline void advancePhase(float& phase, const float increment) noexcept
{
    phase += increment;
    phase -= std::floor(phase);
}

class Envelope
{
public:
    enum class Stage { Idle, Attack, Decay, Sustain, Release };

    void noteOn() noexcept
    {
        stage = Stage::Attack;
        value = clampf(value, 0.0f, 1.0f);
    }

    void noteOff() noexcept
    {
        if (stage != Stage::Idle)
        {
            stage = Stage::Release;
            releaseStart = value;
        }
    }

    void reset() noexcept
    {
        stage = Stage::Idle;
        value = 0.0f;
        releaseStart = 0.0f;
    }

    float process(const float sampleRate, const float attack, const float decay,
                  const float sustain, const float release) noexcept
    {
        switch (stage)
        {
        case Stage::Idle:
            value = 0.0f;
            break;
        case Stage::Attack:
            value += 1.0f / std::max(1.0f, attack * sampleRate);
            if (value >= 1.0f)
            {
                value = 1.0f;
                stage = Stage::Decay;
            }
            break;
        case Stage::Decay:
            value -= (1.0f - sustain) / std::max(1.0f, decay * sampleRate);
            if (value <= sustain)
            {
                value = sustain;
                stage = Stage::Sustain;
            }
            break;
        case Stage::Sustain:
            value = sustain;
            break;
        case Stage::Release:
            value -= releaseStart / std::max(1.0f, release * sampleRate);
            if (value <= 0.00001f)
            {
                value = 0.0f;
                stage = Stage::Idle;
            }
            break;
        }
        return value;
    }

    bool isIdle() const noexcept { return stage == Stage::Idle; }

private:
    Stage stage = Stage::Idle;
    float value = 0.0f;
    float releaseStart = 0.0f;
};

struct StateVariableFilter
{
    float ic1 = 0.0f;
    float ic2 = 0.0f;

    void reset() noexcept { ic1 = ic2 = 0.0f; }

    float processLowPass(const float input, const float cutoff,
                         const float resonance, const float sampleRate) noexcept
    {
        const float safeCutoff = clampf(cutoff, 20.0f, sampleRate * 0.45f);
        const float g = std::tan(kPi * safeCutoff / sampleRate);
        const float k = 2.0f - 1.92f * clampf(resonance, 0.0f, 1.0f);
        const float a1 = 1.0f / (1.0f + g * (g + k));
        const float v1 = a1 * (ic1 + g * (input - ic2));
        const float v2 = ic2 + g * v1;
        ic1 = 2.0f * v1 - ic1;
        ic2 = 2.0f * v2 - ic2;
        return v2;
    }
};

struct Voice
{
    bool active = false;
    bool held = false;
    int note = -1;
    float velocity = 0.0f;
    uint64_t age = 0;
    std::array<float, kMaxUnison> phases {};
    float phase2 = 0.0f;
    float subPhase = 0.0f;
    float vibratoPhase = 0.0f;
    uint32_t noiseState = 0x12345678u;
    Envelope ampEnv;
    Envelope filterEnv;
    StateVariableFilter filterL;
    StateVariableFilter filterR;

    void start(const int newNote, const float newVelocity, const uint64_t newAge) noexcept
    {
        active = true;
        held = true;
        note = newNote;
        velocity = newVelocity;
        age = newAge;
        for (uint32_t i = 0; i < kMaxUnison; ++i)
            phases[i] = std::fmod(0.117f * static_cast<float>(i + 1u)
                                 + 0.019f * static_cast<float>(newNote), 1.0f);
        phase2 = 0.193f;
        subPhase = 0.417f;
        vibratoPhase = 0.0f;
        noiseState ^= static_cast<uint32_t>(newNote * 2654435761u + newAge);
        filterL.reset();
        filterR.reset();
        ampEnv.noteOn();
        filterEnv.noteOn();
    }

    void release() noexcept
    {
        held = false;
        ampEnv.noteOff();
        filterEnv.noteOff();
    }

    float noise() noexcept
    {
        noiseState ^= noiseState << 13u;
        noiseState ^= noiseState >> 17u;
        noiseState ^= noiseState << 5u;
        return static_cast<float>(static_cast<int32_t>(noiseState)) / 2147483648.0f;
    }
};

struct DelayLine
{
    std::vector<float> data;
    uint32_t write = 0;

    void resize(const uint32_t size)
    {
        data.assign(std::max<uint32_t>(2u, size), 0.0f);
        write = 0;
    }

    void clear() noexcept
    {
        std::fill(data.begin(), data.end(), 0.0f);
        write = 0;
    }

    float read(const float delaySamples) const noexcept
    {
        if (data.empty()) return 0.0f;
        float position = static_cast<float>(write) - delaySamples;
        while (position < 0.0f) position += static_cast<float>(data.size());
        const uint32_t i0 = static_cast<uint32_t>(position) % static_cast<uint32_t>(data.size());
        const uint32_t i1 = (i0 + 1u) % static_cast<uint32_t>(data.size());
        const float frac = position - std::floor(position);
        return data[i0] + (data[i1] - data[i0]) * frac;
    }

    void push(const float sample) noexcept
    {
        if (data.empty()) return;
        data[write] = sample;
        write = (write + 1u) % static_cast<uint32_t>(data.size());
    }
};

struct Comb
{
    DelayLine line;
    float filterStore = 0.0f;

    void resize(const uint32_t size) { line.resize(size); filterStore = 0.0f; }
    void clear() noexcept { line.clear(); filterStore = 0.0f; }

    float process(const float input, const float feedback, const float damp) noexcept
    {
        const float delayed = line.read(static_cast<float>(line.data.size() - 1u));
        filterStore = delayed * (1.0f - damp) + filterStore * damp;
        line.push(input + filterStore * feedback);
        return delayed;
    }
};

} // namespace

class MakerTrancePlugin final : public Plugin
{
public:
    MakerTrancePlugin()
        : Plugin(kParameterCount, 6, 0)
    {
        for (uint32_t i = 0; i < kParameterCount; ++i)
            fParameters[i] = kParameterSpecs[i].def;
        rebuildPattern();
    }

protected:
    const char* getLabel() const override { return "MakerTrance"; }
    const char* getDescription() const override
    {
        return "Trance synthesizer with one-shot MIDI melody generation for the host piano roll";
    }
    const char* getMaker() const override { return "Alza Produz"; }
    const char* getHomePage() const override { return "https://github.com/nikolateslagmk"; }
    const char* getLicense() const override { return "Project source license plus DPF attribution"; }
    uint32_t getVersion() const override { return d_version(1, 1, 0); }
    int64_t getUniqueId() const override { return d_cconst('M', 'T', 'A', '2'); }

    void initParameter(const uint32_t index, Parameter& parameter) override
    {
        if (index >= kParameterCount) return;
        const ParameterSpec& spec = kParameterSpecs[index];
        parameter.hints = kParameterIsAutomatable;
        if ((spec.flags & flagInteger) != 0) parameter.hints |= kParameterIsInteger;
        if ((spec.flags & flagBoolean) != 0) parameter.hints |= kParameterIsBoolean;
        if ((spec.flags & flagLog) != 0) parameter.hints |= kParameterIsLogarithmic;
        parameter.name = spec.name;
        parameter.symbol = spec.symbol;
        parameter.unit = spec.unit;
        parameter.ranges.min = spec.min;
        parameter.ranges.max = spec.max;
        parameter.ranges.def = spec.def;
    }

    float getParameterValue(const uint32_t index) const override
    {
        return index < kParameterCount ? fParameters[index] : 0.0f;
    }

    void setParameterValue(const uint32_t index, const float value) override
    {
        if (index >= kParameterCount) return;
        const ParameterSpec& spec = kParameterSpecs[index];
        float v = clampf(value, spec.min, spec.max);
        if ((spec.flags & flagInteger) != 0) v = std::round(v);
        fParameters[index] = v;

        if (index >= pStyle && index <= pHumanize)
            rebuildPattern();
        else if (index == pSeedLow || index == pSeedHigh)
            rebuildPattern();

        if (index == pMode)
        {
            allNotesOff(true);
            clearEffectTails();
            resetSequencer();
        }
    }

    void initProgramName(const uint32_t index, String& programName) override
    {
        static constexpr const char* names[6] = {
            "Uplifting Spirit", "Uplifting Pluck", "Psy Cosmic Acid",
            "Psy Astral Sequence", "Progressive Horizon", "Progressive Deep Flow"
        };
        if (index < 6u) programName = names[index];
    }

    void loadProgram(const uint32_t index) override
    {
        for (uint32_t i = 0; i < kParameterCount; ++i)
            fParameters[i] = kParameterSpecs[i].def;

        switch (index)
        {
        case 0:
            fParameters[pStyle] = 0.0f; fParameters[pScale] = 4.0f;
            fParameters[pCutoff] = 12400.0f; fParameters[pReverbMix] = 0.38f;
            fParameters[pAmpR] = 0.82f; fParameters[pDensity] = 0.82f;
            break;
        case 1:
            fParameters[pStyle] = 0.0f; fParameters[pScale] = 1.0f;
            fParameters[pAmpD] = 0.16f; fParameters[pAmpS] = 0.34f; fParameters[pAmpR] = 0.23f;
            fParameters[pFilterD] = 0.22f; fParameters[pGateLength] = 0.32f;
            break;
        case 2:
            fParameters[pStyle] = 1.0f; fParameters[pScale] = 3.0f;
            fParameters[pColor] = 0.82f; fParameters[pResonance] = 0.58f;
            fParameters[pDrive] = 2.15f; fParameters[pGateLength] = 0.28f;
            fParameters[pDelayMix] = 0.16f;
            break;
        case 3:
            fParameters[pStyle] = 1.0f; fParameters[pScale] = 5.0f;
            fParameters[pRate] = 2.0f; fParameters[pDensity] = 0.90f;
            fParameters[pGateDepth] = 0.34f; fParameters[pMotion] = 0.86f;
            break;
        case 4:
            fParameters[pStyle] = 2.0f; fParameters[pScale] = 2.0f;
            fParameters[pUnison] = 5.0f; fParameters[pDetune] = 15.0f;
            fParameters[pCutoff] = 7200.0f; fParameters[pReverbMix] = 0.27f;
            fParameters[pDensity] = 0.68f;
            break;
        default:
            fParameters[pStyle] = 2.0f; fParameters[pScale] = 0.0f;
            fParameters[pAmpA] = 0.12f; fParameters[pAmpR] = 1.25f;
            fParameters[pCutoff] = 4800.0f; fParameters[pChorus] = 0.36f;
            fParameters[pMotion] = 0.38f;
            break;
        }

        rebuildPattern();
        allNotesOff();
        resetSequencer();
    }

    void activate() override
    {
        configureForSampleRate(static_cast<float>(getSampleRate()));
        resetAll();
    }

    void deactivate() override { resetAll(); }

    void run(const float**, float** outputs, const uint32_t frames,
             const MidiEvent* midiEvents, const uint32_t midiEventCount) override
    {
        float* const outL = outputs[0];
        float* const outR = outputs[1];
        std::memset(outL, 0, sizeof(float) * frames);
        std::memset(outR, 0, sizeof(float) * frames);

        const float currentRate = static_cast<float>(getSampleRate());
        if (std::abs(currentRate - fSampleRate) > 0.5f)
            configureForSampleRate(currentRate);

        double bpm = 138.0;
        const TimePosition& timePosition = getTimePosition();
        if (timePosition.bbt.valid && timePosition.bbt.beatsPerMinute > 20.0)
            bpm = timePosition.bbt.beatsPerMinute;
        bpm = std::max(40.0, std::min(300.0, bpm));

        const bool automatic = fParameters[pMode] >= 0.5f;
        const bool transportPlaying = timePosition.playing;

        // AUTO is a one-shot MIDI capture mode. It never triggers the internal
        // synthesizer. FL Studio can burn the emitted notes to the Piano roll.
        if (automatic && transportPlaying && !fWasPlaying)
            resetSequencer();
        if (automatic && !transportPlaying && fWasPlaying)
            releaseAutoNote(0u, true);
        if (!automatic && fWasPlaying)
            releaseAutoNote(0u, true);
        fWasPlaying = automatic && transportPlaying;

        uint32_t eventIndex = 0;
        for (uint32_t frame = 0; frame < frames; ++frame)
        {
            while (eventIndex < midiEventCount && midiEvents[eventIndex].frame <= frame)
            {
                handleMidi(midiEvents[eventIndex], automatic);
                ++eventIndex;
            }

            if (automatic && transportPlaying)
                processSequencer(frame, bpm);

            float left = 0.0f;
            float right = 0.0f;
            for (Voice& voice : fVoices)
            {
                if (voice.active)
                    renderVoice(voice, left, right);
            }

            left = std::tanh(left * fParameters[pDrive]);
            right = std::tanh(right * fParameters[pDrive]);

            applyTranceGate(left, right, bpm);
            applyChorus(left, right);
            applyDelay(left, right, bpm);
            applyReverb(left, right);

            const float master = fParameters[pMaster];
            outL[frame] = clampf(std::tanh(left * master), -1.0f, 1.0f);
            outR[frame] = clampf(std::tanh(right * master), -1.0f, 1.0f);
        }
    }

private:
    void rebuildPattern() noexcept
    {
        fPattern = generateMelody(configFromParameters(fParameters));
    }

    void configureForSampleRate(const float rate)
    {
        fSampleRate = std::max(8000.0f, rate);
        fDelayL.resize(static_cast<uint32_t>(fSampleRate * 4.0f) + 8u);
        fDelayR.resize(static_cast<uint32_t>(fSampleRate * 4.0f) + 8u);
        fChorusL.resize(static_cast<uint32_t>(fSampleRate * 0.08f) + 8u);
        fChorusR.resize(static_cast<uint32_t>(fSampleRate * 0.08f) + 8u);

        constexpr std::array<float, 4> timesL {{0.0297f, 0.0371f, 0.0411f, 0.0437f}};
        constexpr std::array<float, 4> timesR {{0.0307f, 0.0383f, 0.0427f, 0.0451f}};
        for (uint32_t i = 0; i < 4u; ++i)
        {
            fCombsL[i].resize(static_cast<uint32_t>(fSampleRate * timesL[i]) + 2u);
            fCombsR[i].resize(static_cast<uint32_t>(fSampleRate * timesR[i]) + 2u);
        }
    }

    void resetAll() noexcept
    {
        allNotesOff(true);
        fDelayL.clear(); fDelayR.clear();
        fChorusL.clear(); fChorusR.clear();
        for (Comb& comb : fCombsL) comb.clear();
        for (Comb& comb : fCombsR) comb.clear();
        fPitchBend = 0.0f;
        fGatePhase = 0.0;
        fSmoothedGate = 1.0f;
        fChorusPhase = 0.0f;
        fWasPlaying = false;
        resetSequencer();
    }

    void clearEffectTails() noexcept
    {
        fDelayL.clear(); fDelayR.clear();
        fChorusL.clear(); fChorusR.clear();
        for (Comb& comb : fCombsL) comb.clear();
        for (Comb& comb : fCombsR) comb.clear();
        fGatePhase = 0.0;
        fSmoothedGate = 1.0f;
        fChorusPhase = 0.0f;
    }

    void resetSequencer() noexcept
    {
        fSequenceStep = 0u;
        fSamplesUntilStep = 0.0;
        fAutoGateSamples = 0.0;
        fAutoNote = -1;
        fSequenceFinished = false;
    }

    void handleMidi(const MidiEvent& event, const bool automatic) noexcept
    {
        if (event.size == 0u) return;
        const uint8_t* const data = event.data;
        const uint8_t status = data[0] & 0xF0u;

        if (status == 0x90u && event.size >= 3u)
        {
            const int note = data[1] & 0x7F;
            const int velocity = data[2] & 0x7F;
            if (!automatic)
            {
                if (velocity == 0) noteOff(note);
                else noteOn(note, static_cast<float>(velocity) / 127.0f);
            }
        }
        else if (status == 0x80u && event.size >= 3u)
        {
            if (!automatic) noteOff(data[1] & 0x7F);
        }
        else if (status == 0xE0u && event.size >= 3u)
        {
            const int value = static_cast<int>(data[1]) | (static_cast<int>(data[2]) << 7);
            fPitchBend = (static_cast<float>(value) - 8192.0f) / 8192.0f * 2.0f;
        }
        else if (status == 0xB0u && event.size >= 3u && data[1] == 123u)
        {
            allNotesOff();
        }
    }

    void processSequencer(const uint32_t frame, const double bpm) noexcept
    {
        if (fSequenceFinished) return;

        if (fAutoGateSamples > 0.0)
        {
            fAutoGateSamples -= 1.0;
            if (fAutoGateSamples <= 0.0)
                releaseAutoNote(frame, true);
        }

        fSamplesUntilStep -= 1.0;
        if (fSamplesUntilStep > 0.0) return;

        releaseAutoNote(frame, true);

        // Do not loop. Each transport start emits exactly one generated phrase.
        if (fSequenceStep >= fPattern.length)
        {
            fSequenceFinished = true;
            return;
        }

        const MelodyStep& step = fPattern.steps[fSequenceStep];
        const double duration = stepDurationSamples(
            static_cast<double>(fSampleRate), bpm,
            fParameters[pRate], fParameters[pSwing], fSequenceStep);
        fSamplesUntilStep += duration;

        if (step.active != 0u)
        {
            fAutoNote = clampi(static_cast<int>(step.note), 0, 127);
            fAutoGateSamples = std::max(1.0, duration * (static_cast<double>(step.gate) / 127.0));
            sendMidi(frame, true, fAutoNote, step.velocity);
        }

        ++fSequenceStep;
    }

    void sendMidi(const uint32_t frame, const bool noteOnEvent,
                  const int note, const int velocity) noexcept
    {
        MidiEvent event {};
        event.frame = frame;
        event.size = 3u;
        event.data[0] = noteOnEvent ? 0x90u : 0x80u;
        event.data[1] = static_cast<uint8_t>(clampi(note, 0, 127));
        event.data[2] = static_cast<uint8_t>(clampi(velocity, 0, 127));
        writeMidiEvent(event);
    }

    void releaseAutoNote(const uint32_t frame, const bool sendOutput) noexcept
    {
        if (fAutoNote < 0) return;
        if (sendOutput) sendMidi(frame, false, fAutoNote, 0);
        fAutoNote = -1;
        fAutoGateSamples = 0.0;
    }

    void noteOn(const int note, const float velocity) noexcept
    {
        Voice* selected = nullptr;
        for (Voice& voice : fVoices)
        {
            if (!voice.active)
            {
                selected = &voice;
                break;
            }
        }
        if (selected == nullptr)
        {
            selected = &*std::min_element(fVoices.begin(), fVoices.end(),
                [](const Voice& a, const Voice& b) { return a.age < b.age; });
        }
        selected->start(note, velocity, ++fVoiceAge);
    }

    void noteOff(const int note) noexcept
    {
        for (Voice& voice : fVoices)
            if (voice.active && voice.note == note && voice.held)
                voice.release();
    }

    void allNotesOff(const bool immediate = false) noexcept
    {
        for (Voice& voice : fVoices)
        {
            if (immediate)
            {
                voice.active = false;
                voice.held = false;
                voice.ampEnv.reset();
                voice.filterEnv.reset();
                voice.filterL.reset();
                voice.filterR.reset();
            }
            else if (voice.active)
            {
                voice.release();
            }
        }
        fAutoNote = -1;
        fAutoGateSamples = 0.0;
    }

    void renderVoice(Voice& voice, float& outL, float& outR) noexcept
    {
        const int style = clampi(static_cast<int>(std::lround(fParameters[pStyle])), 0, 2);
        const float amp = voice.ampEnv.process(fSampleRate,
            fParameters[pAmpA], fParameters[pAmpD], fParameters[pAmpS], fParameters[pAmpR]);
        const float filterSustain = style == 1 ? 0.08f : (style == 2 ? 0.36f : 0.22f);
        const float filterEnvelope = voice.filterEnv.process(fSampleRate,
            0.004f, fParameters[pFilterD], filterSustain, fParameters[pAmpR] * 0.65f);

        if (voice.ampEnv.isIdle())
        {
            voice.active = false;
            return;
        }

        const float vibratoDepth = fParameters[pVibrato] * (style == 0 ? 0.30f : 0.16f);
        const float vibratoRate = style == 1 ? 6.2f : 5.1f;
        const float vibrato = std::sin(kTwoPi * voice.vibratoPhase) * vibratoDepth;
        advancePhase(voice.vibratoPhase, vibratoRate / fSampleRate);
        const float baseHz = midiNoteToHz(voice.note) * std::pow(2.0f, (fPitchBend + vibrato) / 12.0f);

        const uint32_t unison = static_cast<uint32_t>(clampi(static_cast<int>(std::lround(fParameters[pUnison])), 1, 9));
        const float detune = fParameters[pDetune] * (style == 1 ? 0.72f : 1.0f);
        const float spread = fParameters[pSpread];
        const float color = fParameters[pColor];
        float left = 0.0f;
        float right = 0.0f;

        for (uint32_t i = 0; i < unison; ++i)
        {
            const float position = unison == 1u ? 0.0f
                : (static_cast<float>(i) / static_cast<float>(unison - 1u) * 2.0f - 1.0f);
            const float hz = baseHz * std::pow(2.0f, (position * detune) / 1200.0f);
            const float phase = voice.phases[i];
            const float saw = fastSaw(phase);
            const float triangle = fastTriangle(phase);
            const float square = fastSquare(phase);
            float oscillator = saw;

            if (style == 0)
                oscillator = saw * (0.88f + color * 0.12f) + triangle * (0.12f - color * 0.08f);
            else if (style == 1)
            {
                const float phaseWarp = std::sin(kTwoPi * phase * 2.0f) * (0.08f + color * 0.10f);
                oscillator = saw * (0.75f - color * 0.15f)
                           + square * (0.18f + color * 0.22f)
                           + phaseWarp * 0.18f;
            }
            else
                oscillator = triangle * (0.42f - color * 0.18f)
                           + saw * (0.58f + color * 0.18f);

            advancePhase(voice.phases[i], hz / fSampleRate);
            const float pan = position * spread;
            const float angle = (pan + 1.0f) * (kPi * 0.25f);
            left += oscillator * std::cos(angle);
            right += oscillator * std::sin(angle);
        }

        const float normalization = 1.0f / std::sqrt(static_cast<float>(unison));
        left *= normalization;
        right *= normalization;

        const float osc2Hz = baseHz * std::pow(2.0f, fParameters[pOsc2Semi] / 12.0f);
        const float osc2 = style == 1
            ? (fastSquare(voice.phase2) * 0.62f + fastSaw(voice.phase2) * 0.38f)
            : (fastSaw(voice.phase2) * 0.72f + fastTriangle(voice.phase2) * 0.28f);
        advancePhase(voice.phase2, osc2Hz / fSampleRate);
        left += osc2 * fParameters[pOsc2Mix] * 0.68f;
        right += osc2 * fParameters[pOsc2Mix] * 0.68f;

        const float sub = std::sin(kTwoPi * voice.subPhase) * fParameters[pSub];
        advancePhase(voice.subPhase, baseHz * 0.5f / fSampleRate);
        const float noise = voice.noise() * fParameters[pNoise];
        left += sub + noise;
        right += sub - noise * 0.31f;

        const float envOctaves = fParameters[pFilterEnv] * filterEnvelope * (style == 1 ? 7.2f : 6.0f);
        const float styleCutoff = style == 1 ? 0.82f : (style == 2 ? 0.88f : 1.0f);
        const float cutoff = fParameters[pCutoff] * styleCutoff * std::pow(2.0f, envOctaves);
        const float resonance = clampf(fParameters[pResonance] + (style == 1 ? 0.10f : 0.0f), 0.0f, 1.0f);
        left = voice.filterL.processLowPass(left, cutoff, resonance, fSampleRate);
        right = voice.filterR.processLowPass(right, cutoff, resonance, fSampleRate);

        const float gain = amp * (0.52f + voice.velocity * 0.48f) * 0.145f;
        outL += left * gain;
        outR += right * gain;
    }

    void applyTranceGate(float& left, float& right, const double bpm) noexcept
    {
        const float depth = fParameters[pGateDepth];
        if (depth <= 0.0001f) return;

        static constexpr std::array<float, 16> uplifting {{
            1.00f, 0.24f, 0.82f, 0.38f, 1.00f, 0.42f, 0.74f, 0.20f,
            1.00f, 0.28f, 0.90f, 0.46f, 1.00f, 0.52f, 0.78f, 0.18f
        }};
        static constexpr std::array<float, 16> psy {{
            1.00f, 0.12f, 0.64f, 0.08f, 0.86f, 0.18f, 1.00f, 0.10f,
            0.72f, 0.14f, 0.92f, 0.06f, 1.00f, 0.24f, 0.68f, 0.10f
        }};
        static constexpr std::array<float, 16> progressive {{
            1.00f, 0.58f, 0.82f, 0.46f, 0.94f, 0.62f, 0.76f, 0.42f,
            1.00f, 0.54f, 0.88f, 0.48f, 0.92f, 0.66f, 0.80f, 0.44f
        }};

        const double stepsPerSecond = bpm / 60.0 * 4.0;
        fGatePhase += stepsPerSecond / static_cast<double>(fSampleRate);
        fGatePhase -= std::floor(fGatePhase / 16.0) * 16.0;
        const uint32_t step = static_cast<uint32_t>(fGatePhase) & 15u;
        const int style = clampi(static_cast<int>(std::lround(fParameters[pStyle])), 0, 2);
        const float patternValue = style == 1 ? psy[step] : (style == 2 ? progressive[step] : uplifting[step]);
        const float target = (1.0f - depth) + patternValue * depth;
        const float smoothing = 1.0f - std::exp(-1.0f / (0.0035f * fSampleRate));
        fSmoothedGate += (target - fSmoothedGate) * smoothing;
        left *= fSmoothedGate;
        right *= fSmoothedGate;
    }

    void applyChorus(float& left, float& right) noexcept
    {
        const float mix = fParameters[pChorus] * 0.48f;
        if (mix <= 0.0001f)
        {
            fChorusL.push(left);
            fChorusR.push(right);
            return;
        }
        const float lfoL = 0.5f + 0.5f * std::sin(kTwoPi * fChorusPhase);
        const float lfoR = 0.5f + 0.5f * std::sin(kTwoPi * (fChorusPhase + 0.37f));
        advancePhase(fChorusPhase, 0.19f / fSampleRate);
        const float wetL = fChorusL.read((0.012f + lfoL * 0.007f) * fSampleRate);
        const float wetR = fChorusR.read((0.013f + lfoR * 0.008f) * fSampleRate);
        fChorusL.push(left + wetR * 0.08f);
        fChorusR.push(right + wetL * 0.08f);
        left = left * (1.0f - mix) + wetL * mix;
        right = right * (1.0f - mix) + wetR * mix;
    }

    void applyDelay(float& left, float& right, const double bpm) noexcept
    {
        static constexpr std::array<double, 5> divisions {{0.25, 0.375, 0.5, 0.75, 1.0}};
        const int index = clampi(static_cast<int>(std::lround(fParameters[pDelayDivision])), 0, 4);
        const float delaySamples = static_cast<float>((60.0 / bpm)
            * divisions[static_cast<uint32_t>(index)] * fSampleRate);
        const float wetL = fDelayL.read(delaySamples);
        const float wetR = fDelayR.read(delaySamples * 1.007f);
        const float feedback = fParameters[pDelayFeedback];
        const float mix = fParameters[pDelayMix];
        fDelayL.push(left + wetR * feedback);
        fDelayR.push(right + wetL * feedback);
        left = left * (1.0f - mix) + wetL * mix;
        right = right * (1.0f - mix) + wetR * mix;
    }

    void applyReverb(float& left, float& right) noexcept
    {
        const float mix = fParameters[pReverbMix] * 0.60f;
        if (mix <= 0.0001f) return;
        const float feedback = 0.60f + fParameters[pReverbSize] * 0.32f;
        const float damp = 0.08f + fParameters[pReverbDamp] * 0.82f;
        float wetL = 0.0f;
        float wetR = 0.0f;
        const float input = (left + right) * 0.23f;
        for (uint32_t i = 0; i < 4u; ++i)
        {
            wetL += fCombsL[i].process(input + right * 0.04f, feedback, damp);
            wetR += fCombsR[i].process(input + left * 0.04f, feedback, damp);
        }
        wetL *= 0.25f;
        wetR *= 0.25f;
        left = left * (1.0f - mix) + wetL * mix;
        right = right * (1.0f - mix) + wetR * mix;
    }

    std::array<float, kParameterCount> fParameters {};
    MelodyPattern fPattern {};
    std::array<Voice, kVoiceCount> fVoices {};
    uint64_t fVoiceAge = 0u;
    float fSampleRate = 44100.0f;
    float fPitchBend = 0.0f;

    DelayLine fDelayL;
    DelayLine fDelayR;
    DelayLine fChorusL;
    DelayLine fChorusR;
    std::array<Comb, 4> fCombsL;
    std::array<Comb, 4> fCombsR;

    double fGatePhase = 0.0;
    float fSmoothedGate = 1.0f;
    float fChorusPhase = 0.0f;

    uint32_t fSequenceStep = 0u;
    double fSamplesUntilStep = 0.0;
    double fAutoGateSamples = 0.0;
    int fAutoNote = -1;
    bool fWasPlaying = false;
    bool fSequenceFinished = false;
};

Plugin* createPlugin()
{
    return new MakerTrancePlugin();
}

END_NAMESPACE_DISTRHO
