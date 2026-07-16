#include "../Source/MakerTranceShared.hpp"

#include <cmath>
#include <cstdint>
#include <iostream>
#include <set>
#include <sstream>
#include <string>

static std::string serialize(const MakerTrance::MelodyPattern& p)
{
    std::ostringstream out;
    out << p.length << ':';
    for (uint32_t i = 0; i < p.length; ++i)
    {
        const auto& s = p.steps[i];
        out << static_cast<int>(s.note) << ','
            << static_cast<int>(s.velocity) << ','
            << static_cast<int>(s.gate) << ','
            << static_cast<int>(s.active) << ';';
    }
    return out.str();
}

int main()
{
    std::set<std::string> patterns;
    for (uint32_t generation = 1; generation <= 20000; ++generation)
    {
        MakerTrance::MelodyConfig cfg;
        cfg.style = static_cast<int>(generation % 3u);
        cfg.root = 36 + static_cast<int>(generation % 36u);
        cfg.scale = static_cast<int>(generation % 6u);
        cfg.octaves = 1 + static_cast<int>(generation % 3u);
        cfg.length = generation % 3u == 0u ? 8u : (generation % 3u == 1u ? 16u : 32u);
        cfg.density = 0.20f + static_cast<float>(generation % 80u) / 100.0f;
        cfg.motion = static_cast<float>(generation % 101u) / 100.0f;
        cfg.gateLength = 0.15f + static_cast<float>(generation % 75u) / 100.0f;
        cfg.humanize = static_cast<float>(generation % 90u) / 100.0f;
        cfg.generation = generation;

        const auto pattern = MakerTrance::generateMelody(cfg);
        if (pattern.length != cfg.length) return 2;
        if (!patterns.insert(serialize(pattern)).second)
        {
            std::cerr << "duplicate at generation " << generation << '\n';
            return 3;
        }
        for (uint32_t i = 0; i < pattern.length; ++i)
        {
            const auto& step = pattern.steps[i];
            if (step.note < 24 || step.note > 108 || step.velocity > 127 || step.gate > 127)
                return 4;
        }
    }

    // BPM following: the same musical step must become shorter at a higher BPM.
    const double at120 = MakerTrance::stepDurationSamples(48000.0, 120.0, 1.0f, 0.0f, 0u);
    const double at144 = MakerTrance::stepDurationSamples(48000.0, 144.0, 1.0f, 0.0f, 0u);
    if (!(at144 < at120)) return 5;
    if (std::abs((at120 / at144) - 1.2) > 0.0001) return 6;

    // Swing keeps a two-step pair equal to two unswung steps.
    const double even = MakerTrance::stepDurationSamples(48000.0, 138.0, 1.0f, 0.20f, 0u);
    const double odd = MakerTrance::stepDurationSamples(48000.0, 138.0, 1.0f, 0.20f, 1u);
    const double plain = MakerTrance::stepDurationSamples(48000.0, 138.0, 1.0f, 0.0f, 0u);
    if (std::abs((even + odd) - plain * 2.0) > 0.001) return 7;

    std::cout << "OK: 20000 unique patterns; BPM and swing timing validated\n";
    return 0;
}
