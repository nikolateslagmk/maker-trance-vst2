#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>

namespace MakerTrance
{
constexpr uint32_t kMaxSteps = 32;
constexpr uint32_t kVoiceCount = 24;
constexpr uint32_t kMaxUnison = 9;
constexpr float kPi = 3.14159265358979323846f;
constexpr float kTwoPi = 6.28318530717958647692f;

enum ParameterId : uint32_t
{
    pMaster = 0,
    pMode,
    pStyle,
    pRoot,
    pScale,
    pRate,
    pLength,
    pDensity,
    pOctaves,
    pMotion,
    pGateLength,
    pSwing,
    pHumanize,
    pSeedLow,
    pSeedHigh,
    pMidiOut,

    pUnison,
    pDetune,
    pSpread,
    pColor,
    pOsc2Mix,
    pOsc2Semi,
    pSub,
    pNoise,
    pDrive,
    pAmpA,
    pAmpD,
    pAmpS,
    pAmpR,
    pCutoff,
    pResonance,
    pFilterEnv,
    pFilterD,
    pVibrato,
    pGateDepth,
    pDelayMix,
    pDelayFeedback,
    pDelayDivision,
    pReverbMix,
    pReverbSize,
    pReverbDamp,
    pChorus,

    pPreviewTrigger,
    pStopTrigger,

    kParameterCount
};

enum ParamFlags : uint8_t
{
    flagNone = 0,
    flagInteger = 1u << 0,
    flagLog = 1u << 1,
    flagBoolean = 1u << 2
};

struct ParameterSpec
{
    const char* name;
    const char* symbol;
    const char* unit;
    float min;
    float max;
    float def;
    uint8_t flags;
};

constexpr std::array<ParameterSpec, kParameterCount> kParameterSpecs {{
    {"Master", "master", "", 0.0f, 1.0f, 0.72f, flagNone},
    {"Play Mode", "mode", "0=Manual synth 1=One-shot MIDI capture", 0.0f, 1.0f, 0.0f, flagInteger | flagBoolean},
    {"Trance Style", "style", "0=Uplifting 1=Psy 2=Progressive", 0.0f, 2.0f, 0.0f, flagInteger},
    {"Root Note", "root", "MIDI", 24.0f, 84.0f, 48.0f, flagInteger},
    {"Scale", "scale", "0=Minor 1=Major 2=Dorian 3=Phrygian 4=Lydian 5=Harmonic Minor", 0.0f, 5.0f, 1.0f, flagInteger},
    {"Sequence Rate", "rate", "0=1/8 1=1/16 2=1/16T 3=1/32", 0.0f, 3.0f, 1.0f, flagInteger},
    {"Sequence Length", "length", "0=8 1=16 2=32", 0.0f, 2.0f, 1.0f, flagInteger},
    {"Melody Density", "density", "", 0.10f, 1.0f, 0.78f, flagNone},
    {"Octave Range", "octaves", "oct", 1.0f, 3.0f, 2.0f, flagInteger},
    {"Melodic Motion", "motion", "", 0.0f, 1.0f, 0.62f, flagNone},
    {"Note Gate", "gate_length", "", 0.08f, 0.98f, 0.55f, flagNone},
    {"Swing", "swing", "", 0.0f, 0.42f, 0.08f, flagNone},
    {"Humanize", "humanize", "", 0.0f, 1.0f, 0.18f, flagNone},
    {"Generation Low", "seed_low", "", 0.0f, 65535.0f, 1.0f, flagInteger},
    {"Generation High", "seed_high", "", 0.0f, 65535.0f, 0.0f, flagInteger},
    {"MIDI Output", "midi_out", "", 0.0f, 1.0f, 1.0f, flagInteger | flagBoolean},

    {"Unison", "unison", "voices", 1.0f, 9.0f, 7.0f, flagInteger},
    {"Detune", "detune", "cents", 0.0f, 65.0f, 24.0f, flagNone},
    {"Stereo Width", "spread", "", 0.0f, 1.0f, 0.84f, flagNone},
    {"Timbre Color", "color", "", 0.0f, 1.0f, 0.55f, flagNone},
    {"Oscillator 2", "osc2_mix", "", 0.0f, 1.0f, 0.28f, flagNone},
    {"Osc 2 Semitone", "osc2_semi", "st", -24.0f, 24.0f, 12.0f, flagInteger},
    {"Sub", "sub", "", 0.0f, 1.0f, 0.14f, flagNone},
    {"Air Noise", "noise", "", 0.0f, 0.45f, 0.018f, flagNone},
    {"Drive", "drive", "", 0.5f, 5.0f, 1.35f, flagNone},
    {"Amp Attack", "amp_a", "s", 0.001f, 4.0f, 0.008f, flagLog},
    {"Amp Decay", "amp_d", "s", 0.001f, 4.0f, 0.24f, flagLog},
    {"Amp Sustain", "amp_s", "", 0.0f, 1.0f, 0.76f, flagNone},
    {"Amp Release", "amp_r", "s", 0.001f, 8.0f, 0.62f, flagLog},
    {"Cutoff", "cutoff", "Hz", 40.0f, 20000.0f, 9200.0f, flagLog},
    {"Resonance", "resonance", "", 0.02f, 1.0f, 0.20f, flagNone},
    {"Filter Envelope", "filter_env", "", -1.0f, 1.0f, 0.52f, flagNone},
    {"Filter Decay", "filter_d", "s", 0.005f, 5.0f, 0.42f, flagLog},
    {"Vibrato", "vibrato", "", 0.0f, 1.0f, 0.08f, flagNone},
    {"Trance Gate", "gate_depth", "", 0.0f, 1.0f, 0.12f, flagNone},
    {"Delay Mix", "delay_mix", "", 0.0f, 1.0f, 0.24f, flagNone},
    {"Delay Feedback", "delay_feedback", "", 0.0f, 0.88f, 0.36f, flagNone},
    {"Delay Division", "delay_division", "0=1/16 1=1/8D 2=1/8 3=1/4D 4=1/4", 0.0f, 4.0f, 2.0f, flagInteger},
    {"Reverb Mix", "reverb_mix", "", 0.0f, 1.0f, 0.30f, flagNone},
    {"Reverb Size", "reverb_size", "", 0.0f, 1.0f, 0.74f, flagNone},
    {"Reverb Damping", "reverb_damp", "", 0.0f, 1.0f, 0.40f, flagNone},
    {"Chorus", "chorus", "", 0.0f, 1.0f, 0.20f, flagNone},

    {"Preview Trigger", "preview_trigger", "", 0.0f, 65535.0f, 0.0f, flagInteger},
    {"Stop Trigger", "stop_trigger", "", 0.0f, 65535.0f, 0.0f, flagInteger}
}};

inline float clampf(const float value, const float lo, const float hi) noexcept
{
    return std::max(lo, std::min(hi, value));
}

inline int clampi(const int value, const int lo, const int hi) noexcept
{
    return std::max(lo, std::min(hi, value));
}

inline uint32_t melodyLengthFromParam(const float value) noexcept
{
    const int mode = clampi(static_cast<int>(std::lround(value)), 0, 2);
    return mode == 0 ? 8u : (mode == 1 ? 16u : 32u);
}

inline double stepsPerBeatFromRate(const float value) noexcept
{
    switch (clampi(static_cast<int>(std::lround(value)), 0, 3))
    {
    case 0: return 2.0;  // 1/8
    case 1: return 4.0;  // 1/16
    case 2: return 6.0;  // 1/16 triplet
    default: return 8.0; // 1/32
    }
}

inline double stepDurationSamples(const double sampleRate, const double bpm,
                                  const float rate, const float swing,
                                  const uint32_t stepIndex) noexcept
{
    const double safeRate = std::max(8000.0, sampleRate);
    const double safeBpm = std::max(20.0, std::min(400.0, bpm));
    const double base = safeRate * 60.0 / safeBpm / stepsPerBeatFromRate(rate);
    const double safeSwing = static_cast<double>(clampf(swing, 0.0f, 0.42f));
    const double swingFactor = (stepIndex & 1u) != 0u ? 1.0 + safeSwing : 1.0 - safeSwing;
    return std::max(1.0, base * swingFactor);
}

inline uint32_t stepDurationTicks(const uint32_t ppq, const float rate,
                                  const float swing, const uint32_t stepIndex) noexcept
{
    const double safePpq = static_cast<double>(std::max<uint32_t>(24u, ppq));
    const double base = safePpq / stepsPerBeatFromRate(rate);
    const double safeSwing = static_cast<double>(clampf(swing, 0.0f, 0.42f));
    const double swingFactor = (stepIndex & 1u) != 0u ? 1.0 + safeSwing : 1.0 - safeSwing;
    return static_cast<uint32_t>(std::max(1.0, std::round(base * swingFactor)));
}

inline uint32_t generationFromValues(const float low, const float high) noexcept
{
    const uint32_t lo = static_cast<uint32_t>(clampi(static_cast<int>(std::lround(low)), 0, 65535));
    const uint32_t hi = static_cast<uint32_t>(clampi(static_cast<int>(std::lround(high)), 0, 65535));
    return lo | (hi << 16u);
}

inline void valuesFromGeneration(const uint32_t generation, float& low, float& high) noexcept
{
    low = static_cast<float>(generation & 0xffffu);
    high = static_cast<float>((generation >> 16u) & 0xffffu);
}

inline float valueToNormalized(const uint32_t index, const float value) noexcept
{
    const ParameterSpec& s = kParameterSpecs[index];
    const float v = clampf(value, s.min, s.max);
    if ((s.flags & flagLog) != 0 && s.min > 0.0f)
        return std::log(v / s.min) / std::log(s.max / s.min);
    return (v - s.min) / std::max(0.000001f, s.max - s.min);
}

inline float normalizedToValue(const uint32_t index, const float normalized) noexcept
{
    const ParameterSpec& s = kParameterSpecs[index];
    const float n = clampf(normalized, 0.0f, 1.0f);
    float value = (s.flags & flagLog) != 0 && s.min > 0.0f
        ? s.min * std::pow(s.max / s.min, n)
        : s.min + (s.max - s.min) * n;
    if ((s.flags & flagInteger) != 0)
        value = std::round(value);
    return clampf(value, s.min, s.max);
}

struct XorShift64Star
{
    explicit XorShift64Star(uint64_t seed) noexcept
        : state(seed != 0u ? seed : 0x9e3779b97f4a7c15ULL) {}

    uint64_t nextU64() noexcept
    {
        uint64_t x = state;
        x ^= x >> 12u;
        x ^= x << 25u;
        x ^= x >> 27u;
        state = x;
        return x * 2685821657736338717ULL;
    }

    uint32_t nextU32() noexcept { return static_cast<uint32_t>(nextU64() >> 32u); }
    float unit() noexcept { return static_cast<float>((nextU32() >> 8u) * (1.0 / 16777216.0)); }
    int range(const int lo, const int hi) noexcept
    {
        if (hi <= lo) return lo;
        return lo + static_cast<int>(nextU32() % static_cast<uint32_t>(hi - lo + 1));
    }

    uint64_t state;
};

struct MelodyStep
{
    int8_t note = 48;
    uint8_t velocity = 96;
    uint8_t gate = 80;
    uint8_t active = 1;
};

struct MelodyPattern
{
    std::array<MelodyStep, kMaxSteps> steps {};
    uint32_t length = 16;
    uint32_t generation = 1;
    uint32_t hash = 0;
};

struct MelodyConfig
{
    int style = 0;
    int root = 48;
    int scale = 1;
    int octaves = 2;
    uint32_t length = 16;
    float density = 0.78f;
    float motion = 0.62f;
    float gateLength = 0.55f;
    float humanize = 0.18f;
    uint32_t generation = 1;
};

constexpr std::array<std::array<int, 7>, 6> kScales {{
    {{0, 2, 3, 5, 7, 8, 10}},  // natural minor
    {{0, 2, 4, 5, 7, 9, 11}},  // major
    {{0, 2, 3, 5, 7, 9, 10}},  // dorian
    {{0, 1, 3, 5, 7, 8, 10}},  // phrygian
    {{0, 2, 4, 6, 7, 9, 11}},  // lydian
    {{0, 2, 3, 5, 7, 8, 11}}   // harmonic minor
}};

inline int noteFromDegree(const MelodyConfig& cfg, int degree) noexcept
{
    const auto& scale = kScales[clampi(cfg.scale, 0, 5)];
    int octave = 0;
    while (degree < 0) { degree += 7; --octave; }
    octave += degree / 7;
    const int scaleIndex = degree % 7;
    return clampi(cfg.root + scale[scaleIndex] + octave * 12, 24, 108);
}

inline uint32_t fnv1a(const MelodyPattern& pattern) noexcept
{
    uint32_t h = 2166136261u;
    for (uint32_t i = 0; i < pattern.length; ++i)
    {
        const MelodyStep& s = pattern.steps[i];
        h = (h ^ static_cast<uint8_t>(s.note)) * 16777619u;
        h = (h ^ s.velocity) * 16777619u;
        h = (h ^ s.gate) * 16777619u;
        h = (h ^ s.active) * 16777619u;
    }
    return h;
}

inline int positiveModulo(const int value, const int modulus) noexcept
{
    const int result = value % modulus;
    return result < 0 ? result + modulus : result;
}

inline int pitchClass(const int midiNote) noexcept
{
    return positiveModulo(midiNote, 12);
}

inline bool isChordTone(const MelodyConfig& cfg, const int midiNote,
                        const int chordRootDegree) noexcept
{
    const int pc = pitchClass(midiNote);
    return pc == pitchClass(noteFromDegree(cfg, chordRootDegree))
        || pc == pitchClass(noteFromDegree(cfg, chordRootDegree + 2))
        || pc == pitchClass(noteFromDegree(cfg, chordRootDegree + 4));
}

inline std::array<int, 4> selectProgression(const MelodyConfig& cfg,
                                             XorShift64Star& rng) noexcept
{
    const int variant = rng.range(0, 2);

    // Every progression ends on a dominant-function chord. The final melody
    // note resolves to the tonic, producing the emotional trance cadence.
    switch (clampi(cfg.scale, 0, 5))
    {
    case 1: // major
        if (variant == 0) return {{0, 5, 3, 4}}; // I vi IV V
        if (variant == 1) return {{5, 3, 0, 4}}; // vi IV I V
        return {{0, 3, 5, 4}};                   // I IV vi V
    case 2: // dorian
        if (variant == 0) return {{0, 3, 6, 4}}; // i IV VII v
        if (variant == 1) return {{0, 6, 3, 4}}; // i VII IV v
        return {{0, 3, 5, 4}};                  // i IV vi v
    case 3: // phrygian
        if (variant == 0) return {{0, 1, 6, 4}}; // i II VII v
        if (variant == 1) return {{0, 5, 1, 4}}; // i VI II v
        return {{0, 1, 5, 4}};                  // i II VI v
    case 4: // lydian
        if (variant == 0) return {{0, 1, 4, 4}}; // I II V V
        if (variant == 1) return {{0, 4, 1, 4}}; // I V II V
        return {{0, 3, 1, 4}};                  // I IV II V
    case 5: // harmonic minor
        if (variant == 0) return {{0, 5, 2, 4}}; // i VI III V
        if (variant == 1) return {{0, 3, 5, 4}}; // i iv VI V
        return {{0, 2, 3, 4}};                  // i III iv V
    default: // natural minor
        if (variant == 0) return {{0, 5, 2, 4}}; // i VI III v
        if (variant == 1) return {{0, 3, 5, 4}}; // i iv VI v
        return {{0, 5, 3, 4}};                  // i VI iv v
    }
}

inline std::array<int, 8> triadMotifByVariant(const int style,
                                               const int rawVariant) noexcept
{
    const int variant = positiveModulo(rawVariant, 4);

    if (style == 0) // uplifting: broad arpeggios and octave lifts
    {
        switch (variant)
        {
        case 0: return {{0, 2, 4, 7, 4, 2, 4, 2}};
        case 1: return {{0, 4, 2, 7, 9, 7, 4, 2}};
        case 2: return {{2, 4, 7, 4, 2, 0, 2, 4}};
        default:return {{0, 2, 4, 9, 7, 4, 2, 1}};
        }
    }

    if (style == 1) // psychedelic: coherent pedal-tone ostinatos
    {
        switch (variant)
        {
        case 0: return {{0, 4, 0, 2, 0, 7, 4, 2}};
        case 1: return {{0, 2, 0, 4, 0, 2, 1, 0}};
        case 2: return {{0, 7, 4, 0, 2, 0, 4, 1}};
        default:return {{0, 4, 2, 0, 7, 4, 2, 0}};
        }
    }

    // progressive: restrained motifs with smooth repetition and variation
    switch (variant)
    {
    case 0: return {{0, 2, 4, 2, 0, 2, 4, 2}};
    case 1: return {{2, 4, 2, 0, 2, 3, 2, 0}};
    case 2: return {{0, 4, 2, 4, 0, 2, 1, 2}};
    default:return {{0, 2, 4, 2, 7, 4, 2, 1}};
    }
}

inline std::array<int, 8> selectTriadMotif(const int style,
                                           XorShift64Star& rng) noexcept
{
    return triadMotifByVariant(style, rng.range(0, 3));
}

inline int nearestVoicedNote(const MelodyConfig& cfg, const int scaleDegree,
                             const int desiredMidi, const int previousMidi) noexcept
{
    int bestNote = noteFromDegree(cfg, scaleDegree);
    int bestCost = 1000000;
    int bestMovement = 1000000;

    for (int octaveShift = -4; octaveShift <= 5; ++octaveShift)
    {
        const int note = noteFromDegree(cfg, scaleDegree + octaveShift * 7);
        const int targetCost = std::abs(note - desiredMidi) * 3;
        const int movement = previousMidi >= 0 ? std::abs(note - previousMidi) : 0;
        int cost = targetCost + movement * 2;
        if (movement > 12) cost += (movement - 12) * 7;
        if (movement == 0) cost += 2;
        if (note < 36 || note > 103) cost += 20;

        if (cost < bestCost || (cost == bestCost && movement < bestMovement))
        {
            bestCost = cost;
            bestMovement = movement;
            bestNote = note;
        }
    }

    return clampi(bestNote, 24, 108);
}

inline int nearestTonic(const MelodyConfig& cfg, const int target,
                        const int previousMidi) noexcept
{
    return nearestVoicedNote(cfg, 0, target, previousMidi);
}

inline float scoreTriadicPhrase(const MelodyConfig& cfg,
                                const MelodyPattern& pattern,
                                const std::array<int, 4>& progression) noexcept
{
    const uint32_t segmentLength = std::max<uint32_t>(1u, pattern.length / 4u);
    int previous = -1;
    int previousDirection = 0;
    int directionChanges = 0;
    int repeatedRun = 0;
    int activeCount = 0;
    int chordToneCount = 0;
    int strongChordToneCount = 0;
    int minNote = 127;
    int maxNote = 0;
    int highestIndex = 0;
    std::array<uint8_t, 12> pitchClasses {};
    float score = 0.0f;

    for (uint32_t i = 0; i < pattern.length; ++i)
    {
        const MelodyStep& step = pattern.steps[i];
        if (step.active == 0u) continue;

        ++activeCount;
        minNote = std::min(minNote, static_cast<int>(step.note));
        if (static_cast<int>(step.note) > maxNote)
        {
            maxNote = static_cast<int>(step.note);
            highestIndex = static_cast<int>(i);
        }
        pitchClasses[static_cast<std::size_t>(pitchClass(step.note))] = 1u;

        const uint32_t chordIndex = std::min<uint32_t>(3u, i / segmentLength);
        const uint32_t local = i % segmentLength;
        const bool strong = local == 0u || (segmentLength >= 4u && local == segmentLength / 2u);
        const bool chordTone = isChordTone(cfg, step.note, progression[chordIndex]);

        if (chordTone)
        {
            ++chordToneCount;
            score += strong ? 7.0f : 3.0f;
            if (strong) ++strongChordToneCount;
        }
        else
        {
            score -= strong ? 10.0f : 1.5f;
        }

        if (previous >= 0)
        {
            const int interval = static_cast<int>(step.note) - previous;
            const int distance = std::abs(interval);
            if (distance == 0)
            {
                ++repeatedRun;
                score += repeatedRun <= 2 ? 0.5f : -3.5f;
            }
            else
            {
                repeatedRun = 0;
                if (distance <= 4) score += 3.0f;
                else if (distance <= 7) score += 1.8f;
                else if (distance <= 12) score -= 1.5f;
                else score -= 11.0f + static_cast<float>(distance - 12);

                const int direction = interval > 0 ? 1 : -1;
                if (previousDirection != 0 && direction != previousDirection)
                    ++directionChanges;
                previousDirection = direction;
            }
        }
        previous = step.note;
    }

    if (activeCount == 0) return -100000.0f;

    const int firstNote = [&pattern]() noexcept {
        for (uint32_t i = 0; i < pattern.length; ++i)
            if (pattern.steps[i].active != 0u) return static_cast<int>(pattern.steps[i].note);
        return 0;
    }();
    const int lastNote = [&pattern]() noexcept {
        for (uint32_t i = pattern.length; i > 0u; --i)
            if (pattern.steps[i - 1u].active != 0u) return static_cast<int>(pattern.steps[i - 1u].note);
        return 0;
    }();

    const int tonicPc = pitchClass(cfg.root);
    const int tonicThirdPc = pitchClass(noteFromDegree(cfg, 2));
    const int tonicFifthPc = pitchClass(noteFromDegree(cfg, 4));
    if (pitchClass(firstNote) == tonicPc) score += 9.0f;
    else if (pitchClass(firstNote) == tonicThirdPc || pitchClass(firstNote) == tonicFifthPc) score += 5.0f;

    if (pitchClass(lastNote) == tonicPc) score += 28.0f;
    else if (pitchClass(lastNote) == tonicThirdPc || pitchClass(lastNote) == tonicFifthPc) score += 10.0f;
    else score -= 15.0f;

    const int range = maxNote - minNote;
    const int preferredMin = cfg.style == 0 ? 10 : (cfg.style == 1 ? 5 : 7);
    const int preferredMax = cfg.style == 0 ? 24 : (cfg.style == 1 ? 15 : 18);
    if (range >= preferredMin && range <= preferredMax) score += 12.0f;
    else score -= static_cast<float>(std::abs(range - clampi(range, preferredMin, preferredMax))) * 1.8f;

    const float climaxPosition = pattern.length > 1u
        ? static_cast<float>(highestIndex) / static_cast<float>(pattern.length - 1u)
        : 0.0f;
    if (climaxPosition >= 0.52f && climaxPosition <= 0.88f) score += 13.0f;
    else score -= 8.0f;

    int classCount = 0;
    for (uint8_t used : pitchClasses) classCount += used != 0u ? 1 : 0;
    if (classCount >= 4 && classCount <= 8) score += 8.0f;
    else score -= 5.0f;

    const float actualDensity = static_cast<float>(activeCount) / static_cast<float>(pattern.length);
    const float desiredDensity = std::max(8.0f / static_cast<float>(pattern.length), cfg.density);
    score -= std::abs(actualDensity - desiredDensity) * 18.0f;

    if (directionChanges >= 2 && directionChanges <= std::max(4, activeCount / 2)) score += 7.0f;
    else if (directionChanges > activeCount * 3 / 4) score -= 10.0f;

    if (chordToneCount * 100 >= activeCount * 78) score += 16.0f;
    if (strongChordToneCount >= 4) score += 8.0f;

    return score;
}

inline MelodyPattern buildTriadicCandidate(const MelodyConfig& cfg,
                                            const uint64_t seed,
                                            std::array<int, 4>& progressionOut) noexcept
{
    XorShift64Star rng(seed);
    const std::array<int, 4> progression = selectProgression(cfg, rng);
    progressionOut = progression;
    const std::array<int, 8> motif = selectTriadMotif(cfg.style, rng);

    MelodyPattern result;
    result.length = cfg.length;
    result.generation = cfg.generation;

    const uint32_t segmentLength = std::max<uint32_t>(1u, cfg.length / 4u);
    int baseRegister = cfg.root + (cfg.style == 0 ? 16 : 12);
    while (baseRegister > 96) baseRegister -= 12;
    while (baseRegister < 48) baseRegister += 12;

    int previousNote = -1;
    int activeCount = 0;

    for (uint32_t i = 0; i < result.length; ++i)
    {
        const uint32_t chordIndex = std::min<uint32_t>(3u, i / segmentLength);
        const uint32_t local = i % segmentLength;
        const uint32_t motifPos = std::min<uint32_t>(7u, (local * 8u) / segmentLength);
        const bool segmentStart = local == 0u;
        const bool strong = segmentStart || (segmentLength >= 4u && local == segmentLength / 2u);
        const float phrase = result.length > 1u
            ? static_cast<float>(i) / static_cast<float>(result.length - 1u)
            : 0.0f;

        int motifOffset = motif[motifPos];
        // Weak passing notes are permitted only as short connectors. Strong
        // positions are always members of the current triad.
        if (strong && positiveModulo(motifOffset, 7) != 0
                   && positiveModulo(motifOffset, 7) != 2
                   && positiveModulo(motifOffset, 7) != 4)
        {
            static constexpr int triadOffsets[3] = {0, 2, 4};
            motifOffset = triadOffsets[rng.range(0, 2)];
        }

        // Reuse the same motif through the progression, with a musical lift
        // near the climax and a cadential descent at the end.
        int scaleDegree = progression[chordIndex] + motifOffset;
        if (cfg.style == 0 && phrase > 0.56f && phrase < 0.84f)
            scaleDegree += 7;
        if (cfg.style == 2 && chordIndex == 2u && motifPos >= 4u)
            scaleDegree += 2;

        const float arch = std::sin(kPi * phrase);
        int contour = 0;
        if (cfg.style == 0)
            contour = static_cast<int>(std::lround(arch * (8.0f + cfg.motion * 9.0f)));
        else if (cfg.style == 1)
            contour = static_cast<int>(std::lround(std::sin(kTwoPi * phrase * 2.0f) * (2.0f + cfg.motion * 2.5f)));
        else
            contour = static_cast<int>(std::lround(arch * (4.0f + cfg.motion * 5.0f)));

        const int desired = baseRegister + contour;
        int note = nearestVoicedNote(cfg, scaleDegree, desired, previousNote);

        // Protect the phrase from random-sounding leaps. Re-voice the same
        // triad tone near the previous note whenever a leap is too large.
        if (previousNote >= 0 && std::abs(note - previousNote) > 10)
            note = nearestVoicedNote(cfg, scaleDegree, previousNote + (note > previousNote ? 5 : -5), previousNote);

        MelodyStep step;
        float activeChance = cfg.density;
        if (cfg.style == 0) activeChance += strong ? 0.18f : 0.02f;
        else if (cfg.style == 1) activeChance += strong ? 0.12f : -0.03f;
        else activeChance += strong ? 0.16f : -0.08f;

        const bool phraseAnchor = i == 0u || i + 1u == result.length || segmentStart;
        step.active = static_cast<uint8_t>(phraseAnchor || rng.unit() < clampf(activeChance, 0.08f, 1.0f));
        step.note = static_cast<int8_t>(note);

        const int baseVelocity = cfg.style == 0 ? 98 : (cfg.style == 1 ? 101 : 92);
        const int accent = strong ? 11 : (local + 1u == segmentLength ? -5 : 0);
        const int velocityJitter = static_cast<int>(std::lround(cfg.humanize * 8.0f));
        step.velocity = static_cast<uint8_t>(clampi(baseVelocity + accent
                                                   + rng.range(-velocityJitter, velocityJitter), 52, 124));

        const int gateBase = static_cast<int>(std::lround(cfg.gateLength * 112.0f));
        const int styleGate = cfg.style == 1 ? -14 : (cfg.style == 2 ? 8 : 2);
        step.gate = static_cast<uint8_t>(clampi(gateBase + styleGate
                                               + rng.range(-5, 5), 16, 124));

        result.steps[i] = step;
        if (step.active != 0u)
        {
            previousNote = note;
            ++activeCount;
        }
    }

    // A recognisable emotional cadence: dominant/fifth approach followed by
    // a tonic resolution. This removes unresolved, arbitrary endings.
    if (result.length >= 2u)
    {
        int beforeLast = -1;
        for (uint32_t i = result.length - 2u; i > 0u; --i)
        {
            if (result.steps[i - 1u].active != 0u)
            {
                beforeLast = result.steps[i - 1u].note;
                break;
            }
        }

        MelodyStep& penultimate = result.steps[result.length - 2u];
        penultimate.active = 1u;
        penultimate.note = static_cast<int8_t>(nearestVoicedNote(
            cfg, 4, baseRegister + 5, beforeLast));
        penultimate.velocity = static_cast<uint8_t>(clampi(94 + rng.range(-3, 3), 60, 120));
        penultimate.gate = static_cast<uint8_t>(clampi(
            static_cast<int>(std::lround(cfg.gateLength * 104.0f)), 24, 116));

        MelodyStep& finalStep = result.steps[result.length - 1u];
        finalStep.active = 1u;
        finalStep.note = static_cast<int8_t>(nearestTonic(
            cfg, baseRegister, penultimate.note));
        finalStep.velocity = static_cast<uint8_t>(clampi(108 + rng.range(-2, 2), 72, 124));
        finalStep.gate = static_cast<uint8_t>(clampi(
            static_cast<int>(std::lround(cfg.gateLength * 118.0f + 8.0f)), 40, 124));
    }

    // Trance phrases need enough rhythmic information to be musical. Keep at
    // least eight active notes, filling only weak positions with triad tones.
    const int minimumActive = std::min<int>(8, static_cast<int>(result.length));
    if (activeCount < minimumActive)
    {
        for (uint32_t i = 1u; i + 2u < result.length && activeCount < minimumActive; ++i)
        {
            MelodyStep& step = result.steps[i];
            if (step.active != 0u) continue;
            const uint32_t chordIndex = std::min<uint32_t>(3u, i / segmentLength);
            const int triadOffset = static_cast<int>((i % 3u) * 2u);
            step.note = static_cast<int8_t>(nearestVoicedNote(
                cfg, progression[chordIndex] + triadOffset,
                baseRegister + static_cast<int>(i % 5u), previousNote));
            step.active = 1u;
            ++activeCount;
        }
    }

    for (uint32_t i = result.length; i < kMaxSteps; ++i)
        result.steps[i] = MelodyStep{};

    return result;
}

inline void applyGenerationMotifVariation(const MelodyConfig& cfg,
                                                MelodyPattern& pattern,
                                                const std::array<int, 4>& progression) noexcept
{
    const uint32_t segmentLength = std::max<uint32_t>(1u, pattern.length / 4u);
    int previousNote = -1;

    for (uint32_t i = 0u; i + 2u < pattern.length; ++i)
    {
        const uint32_t chordIndex = std::min<uint32_t>(3u, i / segmentLength);
        const uint32_t local = i % segmentLength;
        const uint32_t nibble = (cfg.generation >> (chordIndex * 4u)) & 0x0fu;
        const int family = static_cast<int>(nibble & 0x03u);
        const int transform = static_cast<int>((nibble >> 2u) & 0x03u);
        const std::array<int, 8> motif = triadMotifByVariant(cfg.style, family);

        uint32_t motifPos = std::min<uint32_t>(7u, (local * 8u) / segmentLength);
        if (transform == 1)
            motifPos = 7u - motifPos;             // retrograde
        else if (transform == 2)
            motifPos = (motifPos + 2u) & 7u;      // musical rotation

        int offset = motif[motifPos];
        if (transform == 3)
        {
            // Inversion/lift: keep the same triad but place its upper voice
            // toward the phrase climax instead of introducing random notes.
            const int degreeClass = positiveModulo(offset, 7);
            if (degreeClass == 0) offset = 4;
            else if (degreeClass == 4) offset = 7;
            else if (degreeClass == 2 && local >= segmentLength / 2u) offset += 7;
        }

        const int desired = static_cast<int>(pattern.steps[i].note);
        int note = nearestVoicedNote(cfg, progression[chordIndex] + offset,
                                     desired, previousNote);
        if (previousNote >= 0 && std::abs(note - previousNote) > 10)
            note = nearestVoicedNote(cfg, progression[chordIndex] + offset,
                                     previousNote + (note > previousNote ? 5 : -5),
                                     previousNote);

        pattern.steps[i].note = static_cast<int8_t>(note);
        if (pattern.steps[i].active != 0u)
            previousNote = note;
    }

    // Preserve the intentional V-to-I cadence created by the candidate
    // builder. The generation variation never damages the resolution.
}

inline void repairTriadicPhrase(const MelodyConfig& cfg,
                                       MelodyPattern& pattern,
                                       const std::array<int, 4>& progression) noexcept
{
    const uint32_t segmentLength = std::max<uint32_t>(1u, pattern.length / 4u);
    int previous = -1;
    int repeatedRun = 0;

    for (uint32_t i = 0u; i + 2u < pattern.length; ++i)
    {
        MelodyStep& step = pattern.steps[i];
        if (step.active == 0u) continue;

        const int original = static_cast<int>(step.note);
        const int leap = previous >= 0 ? std::abs(original - previous) : 0;
        repeatedRun = previous >= 0 && original == previous ? repeatedRun + 1 : 0;

        if (previous >= 0 && (leap > 9 || repeatedRun >= 2))
        {
            const uint32_t chordIndex = std::min<uint32_t>(3u, i / segmentLength);
            static constexpr int offsets[5] = {0, 2, 4, 7, 9};
            int nextNote = -1;
            for (uint32_t j = i + 1u; j < pattern.length; ++j)
            {
                if (pattern.steps[j].active != 0u)
                {
                    nextNote = pattern.steps[j].note;
                    break;
                }
            }

            int best = original;
            int bestCost = 1000000;
            for (const int offset : offsets)
            {
                const int candidate = nearestVoicedNote(
                    cfg, progression[chordIndex] + offset, original, previous);
                int cost = std::abs(candidate - original) * 2
                         + std::abs(candidate - previous) * 4;
                if (nextNote >= 0) cost += std::abs(candidate - nextNote);
                if (candidate == previous) cost += repeatedRun >= 2 ? 40 : 5;
                if (std::abs(candidate - previous) > 9) cost += 40;
                if (cost < bestCost)
                {
                    bestCost = cost;
                    best = candidate;
                }
            }
            step.note = static_cast<int8_t>(best);
            repeatedRun = best == previous ? repeatedRun : 0;
        }

        previous = step.note;
    }

    if (pattern.length >= 2u)
    {
        int approachPrevious = -1;
        for (uint32_t i = pattern.length - 2u; i > 0u; --i)
        {
            if (pattern.steps[i - 1u].active != 0u)
            {
                approachPrevious = pattern.steps[i - 1u].note;
                break;
            }
        }

        MelodyStep& approach = pattern.steps[pattern.length - 2u];
        MelodyStep& resolution = pattern.steps[pattern.length - 1u];
        resolution.active = 1u;

        static constexpr int cadenceDegrees[5] = {4, 6, 3, 5, 1};
        int bestApproach = approach.note;
        int bestCost = 1000000;
        for (const int degree : cadenceDegrees)
        {
            const int candidate = nearestVoicedNote(
                cfg, degree, approach.note, approachPrevious);
            const int tonic = nearestTonic(cfg, resolution.note, candidate);
            int cost = (approachPrevious >= 0 ? std::abs(candidate - approachPrevious) * 4 : 0)
                     + std::abs(tonic - candidate) * 3
                     + std::abs(candidate - static_cast<int>(approach.note));
            if (approachPrevious >= 0 && std::abs(candidate - approachPrevious) > 9) cost += 60;
            if (std::abs(tonic - candidate) > 12) cost += 60;
            if (cost < bestCost)
            {
                bestCost = cost;
                bestApproach = candidate;
            }
        }

        approach.active = 1u;
        approach.note = static_cast<int8_t>(bestApproach);
        resolution.note = static_cast<int8_t>(nearestTonic(
            cfg, resolution.note, bestApproach));
    }

    int anchor = cfg.root + 12;
    for (uint32_t i = 0u; i < pattern.length; ++i)
    {
        if (pattern.steps[i].active != 0u)
        {
            anchor = pattern.steps[i].note;
            break;
        }
    }
    const int lowLimit = std::max(24, anchor - 5);
    const int highLimit = std::min(108, lowLimit + cfg.octaves * 12 + 7);

    int rangePrevious = -1;
    for (uint32_t i = 0u; i < pattern.length; ++i)
    {
        MelodyStep& step = pattern.steps[i];
        if (step.active == 0u) continue;
        int note = step.note;
        while (note > highLimit && note - 12 >= 24) note -= 12;
        while (note < lowLimit && note + 12 <= 108) note += 12;
        if (rangePrevious >= 0 && std::abs(note - rangePrevious) > 10)
        {
            const int down = note - 12;
            const int up = note + 12;
            if (down >= lowLimit && std::abs(down - rangePrevious) < std::abs(note - rangePrevious))
                note = down;
            else if (up <= highLimit && std::abs(up - rangePrevious) < std::abs(note - rangePrevious))
                note = up;
        }
        step.note = static_cast<int8_t>(clampi(note, 24, 108));
        rangePrevious = step.note;
    }
}

inline MelodyPattern generateMelody(const MelodyConfig& rawConfig) noexcept
{
    MelodyConfig cfg = rawConfig;
    cfg.style = clampi(cfg.style, 0, 2);
    cfg.root = clampi(cfg.root, 24, 84);
    cfg.scale = clampi(cfg.scale, 0, 5);
    cfg.octaves = clampi(cfg.octaves, 1, 3);
    cfg.length = cfg.length == 8u ? 8u : (cfg.length == 32u ? 32u : 16u);
    cfg.density = clampf(cfg.density, 0.10f, 1.0f);
    cfg.motion = clampf(cfg.motion, 0.0f, 1.0f);
    cfg.gateLength = clampf(cfg.gateLength, 0.08f, 0.98f);
    cfg.humanize = clampf(cfg.humanize, 0.0f, 1.0f);

    const uint64_t styleSalt = cfg.style == 0 ? 0xa24baed4963ee407ULL
                             : cfg.style == 1 ? 0x9fb21c651e98df25ULL
                                              : 0xc13fa9a902a6328fULL;
    const uint64_t baseSeed = (static_cast<uint64_t>(cfg.generation) << 32u)
                            ^ styleSalt
                            ^ static_cast<uint64_t>(cfg.root * 131 + cfg.scale * 977);

    MelodyPattern best;
    std::array<int, 4> bestProgression {{0, 5, 2, 4}};
    float bestScore = -1000000.0f;

    // Generate several musically constrained phrases and keep only the best
    // one. This rejection/scoring stage is what prevents random "nada com
    // nada" note streams from reaching the user.
    for (uint32_t attempt = 0u; attempt < 48u; ++attempt)
    {
        std::array<int, 4> progression {};
        const uint64_t candidateSeed = baseSeed
            ^ (0x9e3779b97f4a7c15ULL * static_cast<uint64_t>(attempt + 1u));
        MelodyPattern candidate = buildTriadicCandidate(cfg, candidateSeed, progression);
        const float score = scoreTriadicPhrase(cfg, candidate, progression);
        if (score > bestScore)
        {
            bestScore = score;
            best = candidate;
            bestProgression = progression;
        }
    }

    best.generation = cfg.generation;
    applyGenerationMotifVariation(cfg, best, bestProgression);
    repairTriadicPhrase(cfg, best, bestProgression);

    // Encode the generation number in inaudibly small velocity/gate detail on
    // the first eight active notes. The musical pitches remain untouched, but
    // the complete MIDI event sequence stays unique until the 32-bit counter
    // wraps.
    uint32_t encoded = 0u;
    for (uint32_t i = 0u; i < best.length && encoded < 8u; ++i)
    {
        MelodyStep& step = best.steps[i];
        if (step.active == 0u) continue;
        const uint8_t nibble = static_cast<uint8_t>((cfg.generation >> (encoded * 4u)) & 0x0fu);
        step.velocity = static_cast<uint8_t>((step.velocity & 0xfcu) | (nibble & 0x03u));
        step.gate = static_cast<uint8_t>((step.gate & 0xfcu) | ((nibble >> 2u) & 0x03u));
        ++encoded;
    }

    best.hash = fnv1a(best);
    return best;
}


inline MelodyConfig configFromParameters(const std::array<float, kParameterCount>& p) noexcept
{
    MelodyConfig cfg;
    cfg.style = clampi(static_cast<int>(std::lround(p[pStyle])), 0, 2);
    cfg.root = clampi(static_cast<int>(std::lround(p[pRoot])), 24, 84);
    cfg.scale = clampi(static_cast<int>(std::lround(p[pScale])), 0, 5);
    cfg.octaves = clampi(static_cast<int>(std::lround(p[pOctaves])), 1, 3);
    cfg.length = melodyLengthFromParam(p[pLength]);
    cfg.density = p[pDensity];
    cfg.motion = p[pMotion];
    cfg.gateLength = p[pGateLength];
    cfg.humanize = p[pHumanize];
    cfg.generation = generationFromValues(p[pSeedLow], p[pSeedHigh]);
    return cfg;
}

inline const char* styleName(const int style) noexcept
{
    return style == 1 ? "PSYCHEDELIC" : (style == 2 ? "PROGRESSIVE" : "UPLIFTING");
}

inline const char* scaleName(const int scale) noexcept
{
    switch (clampi(scale, 0, 5))
    {
    case 0: return "MINOR";
    case 1: return "MAJOR";
    case 2: return "DORIAN";
    case 3: return "PHRYGIAN";
    case 4: return "LYDIAN";
    default: return "HARMONIC MIN";
    }
}

inline const char* noteName(const int midi) noexcept
{
    static constexpr const char* names[12] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    return names[(midi % 12 + 12) % 12];
}

} // namespace MakerTrance
