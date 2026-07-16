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
    {"Play Mode", "mode", "0=Manual 1=Auto", 0.0f, 1.0f, 1.0f, flagInteger | flagBoolean},
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
    {"MIDI Output", "midi_out", "", 0.0f, 1.0f, 0.0f, flagInteger | flagBoolean},

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
    {"Chorus", "chorus", "", 0.0f, 1.0f, 0.20f, flagNone}
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
    XorShift64Star rng((static_cast<uint64_t>(cfg.generation) << 32u)
                       ^ styleSalt
                       ^ static_cast<uint64_t>(cfg.root * 131 + cfg.scale * 977));

    MelodyPattern result;
    result.length = cfg.length;
    result.generation = cfg.generation;

    int degree = cfg.style == 1 ? 0 : rng.range(0, 2) * 2;
    int direction = rng.unit() > 0.5f ? 1 : -1;
    const int maxDegree = cfg.octaves * 7 - 1;

    for (uint32_t i = 0; i < result.length; ++i)
    {
        MelodyStep step;
        const float beatAccent = (i % 4u == 0u) ? 0.16f : 0.0f;
        float activeChance = cfg.density + beatAccent;
        if (cfg.style == 1 && (i % 4u == 2u)) activeChance -= 0.12f;
        if (cfg.style == 2 && (i % 8u == 7u)) activeChance -= 0.18f;
        step.active = static_cast<uint8_t>(i == 0u || rng.unit() < clampf(activeChance, 0.08f, 1.0f));

        if (cfg.style == 0) // uplifting: chord tones, arches and octave lifts
        {
            if (i % 8u == 0u && i != 0u)
                direction = -direction;
            const float jumpChance = 0.10f + cfg.motion * 0.28f;
            if (rng.unit() < jumpChance)
            {
                static constexpr int moves[] = {-4, -2, -1, 1, 2, 4, 7};
                degree += moves[rng.range(0, 6)] * direction;
            }
            else
            {
                degree += rng.range(0, 1) == 0 ? direction : 0;
            }
            if (i % 16u == 12u && cfg.octaves > 1)
                degree += 7;
        }
        else if (cfg.style == 1) // psy: pedal notes, semitone tension and syncopation
        {
            const float pedal = 0.46f - cfg.motion * 0.18f;
            if (rng.unit() < pedal)
                degree = (i % 8u == 6u) ? 1 : 0;
            else
            {
                static constexpr int moves[] = {-3, -2, -1, 1, 2, 3, 4};
                degree += moves[rng.range(0, 6)];
            }
            if (i % 4u == 3u && rng.unit() < 0.55f)
                degree = rng.range(0, std::min(5, maxDegree));
        }
        else // progressive: stepwise motifs with restrained variation
        {
            const uint32_t motifPos = i % 8u;
            if (motifPos == 0u)
                degree = rng.range(0, std::min(4, maxDegree));
            else if (motifPos == 4u)
                degree += rng.unit() < 0.5f ? 2 : -2;
            else if (rng.unit() < 0.36f + cfg.motion * 0.40f)
                degree += rng.unit() < 0.5f ? -1 : 1;
        }

        while (degree < 0) degree += 7;
        while (degree > maxDegree) degree -= 7;

        // Favor spiritually consonant anchor tones at phrase boundaries.
        if (i % 8u == 0u)
        {
            static constexpr int anchors[] = {0, 2, 4, 7, 9};
            degree = anchors[rng.range(0, 4)];
            while (degree > maxDegree) degree -= 7;
        }

        step.note = static_cast<int8_t>(noteFromDegree(cfg, degree));
        const int velocityBase = cfg.style == 1 ? 101 : (cfg.style == 0 ? 96 : 91);
        const int velocityJitter = static_cast<int>(std::lround(cfg.humanize * 15.0f));
        step.velocity = static_cast<uint8_t>(clampi(velocityBase + (i % 4u == 0u ? 10 : 0)
                                                   + rng.range(-velocityJitter, velocityJitter), 28, 127));
        const int gateBase = static_cast<int>(std::lround(cfg.gateLength * 112.0f));
        const int styleGate = cfg.style == 1 ? -18 : (cfg.style == 2 ? 10 : 0);
        step.gate = static_cast<uint8_t>(clampi(gateBase + styleGate
                                               + rng.range(-8, 8), 8, 124));
        result.steps[i] = step;
    }

    // Encode the 32-bit generation counter into subtle performance variations.
    // This makes every generated event sequence unique until the counter wraps.
    for (uint32_t i = 0; i < 4u && i < result.length; ++i)
    {
        MelodyStep& s = result.steps[i];
        s.active = 1u;
        const uint8_t byte = static_cast<uint8_t>((cfg.generation >> (i * 8u)) & 0xffu);
        const uint8_t velNibble = byte & 0x0fu;
        const uint8_t gateNibble = (byte >> 4u) & 0x0fu;
        // Fixed ranges make the mapping from the 32-bit generation number injective.
        s.velocity = static_cast<uint8_t>(80u + velNibble);
        s.gate = static_cast<uint8_t>(64u + gateNibble);
    }

    for (uint32_t i = result.length; i < kMaxSteps; ++i)
        result.steps[i] = MelodyStep{};

    result.hash = fnv1a(result);
    return result;
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
