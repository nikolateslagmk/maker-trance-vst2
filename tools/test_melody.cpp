#include "../Source/MakerTranceShared.hpp"
#include "../Source/MakerTranceMidi.hpp"

#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

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

static std::string serializePitch(const MakerTrance::MelodyPattern& p)
{
    std::ostringstream out;
    for (uint32_t i = 0; i < p.length; ++i)
    {
        const auto& s = p.steps[i];
        out << (s.active != 0u ? static_cast<int>(s.note) : -1) << ',';
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

    // Musical quality: scale adherence, tonic resolution, bounded leaps,
    // octave-range respect and no immediate pitch-pattern repetition.
    std::set<std::string> pitchPatterns;
    std::string previousPitchPattern;
    for (uint32_t generation = 1; generation <= 1000; ++generation)
    {
        MakerTrance::MelodyConfig cfg;
        cfg.style = 0;
        cfg.root = 48;
        cfg.scale = 5;
        cfg.octaves = 2;
        cfg.length = 16u;
        cfg.density = 0.78f;
        cfg.motion = 0.62f;
        cfg.generation = generation;

        const auto pattern = MakerTrance::generateMelody(cfg);
        const std::string pitchPattern = serializePitch(pattern);
        if (pitchPattern == previousPitchPattern) return 12;
        previousPitchPattern = pitchPattern;
        pitchPatterns.insert(pitchPattern);

        int previous = -1;
        int minNote = 127;
        int maxNote = 0;
        int active = 0;
        int last = -1;
        for (uint32_t i = 0; i < pattern.length; ++i)
        {
            const auto& step = pattern.steps[i];
            if (step.active == 0u) continue;
            ++active;
            minNote = std::min(minNote, static_cast<int>(step.note));
            maxNote = std::max(maxNote, static_cast<int>(step.note));
            if (previous >= 0 && std::abs(static_cast<int>(step.note) - previous) > 12) return 13;
            previous = step.note;
            last = step.note;

            const int relative = MakerTrance::positiveModulo(static_cast<int>(step.note) - cfg.root, 12);
            bool inScale = false;
            for (const int degree : MakerTrance::kScales[cfg.scale])
                if (degree == relative) inScale = true;
            if (!inScale) return 14;
        }
        if (active < 8) return 15;
        if (MakerTrance::pitchClass(last) != MakerTrance::pitchClass(cfg.root)) return 16;
        if (maxNote - minNote > cfg.octaves * 12 + 7) return 17;
    }
    if (pitchPatterns.size() < 950u) return 18;

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


    // Musical tick timing is independent from project BPM after Piano Roll import.
    const uint32_t tickEven = MakerTrance::stepDurationTicks(480u, 1.0f, 0.20f, 0u);
    const uint32_t tickOdd = MakerTrance::stepDurationTicks(480u, 1.0f, 0.20f, 1u);
    if (tickEven + tickOdd != 240u) return 8;

    // Export a valid Standard MIDI file with MThd/MTrk chunks.
    MakerTrance::MelodyConfig midiCfg;
    midiCfg.generation = 12345u;
    const auto midiPattern = MakerTrance::generateMelody(midiCfg);
    const std::string midiPath = "maker-trance-test.mid";
    if (!MakerTrance::writeMidiFile(midiPath, midiPattern, 1.0f, 0.08f)) return 9;
    std::ifstream midi(midiPath, std::ios::binary);
    std::string header(4, '\0');
    midi.read(&header[0], 4);
    if (header != "MThd") return 10;
    midi.seekg(14, std::ios::beg);
    midi.read(&header[0], 4);
    if (header != "MTrk") return 11;
    midi.close();
    std::remove(midiPath.c_str());

    std::cout << "OK: 20000 unique MIDI patterns; 1000 triadic phrases validated; BPM and MIDI export OK\n";
    return 0;
}
