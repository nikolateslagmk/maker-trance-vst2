#pragma once

#include "MakerTranceShared.hpp"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>

namespace MakerTrance
{
constexpr uint32_t kMidiPpq = 480u;

struct MidiFileEvent
{
    uint32_t tick = 0;
    uint8_t status = 0;
    uint8_t note = 0;
    uint8_t velocity = 0;
};

inline void appendBigEndian16(std::vector<uint8_t>& data, const uint16_t value)
{
    data.push_back(static_cast<uint8_t>((value >> 8u) & 0xffu));
    data.push_back(static_cast<uint8_t>(value & 0xffu));
}

inline void appendBigEndian32(std::vector<uint8_t>& data, const uint32_t value)
{
    data.push_back(static_cast<uint8_t>((value >> 24u) & 0xffu));
    data.push_back(static_cast<uint8_t>((value >> 16u) & 0xffu));
    data.push_back(static_cast<uint8_t>((value >> 8u) & 0xffu));
    data.push_back(static_cast<uint8_t>(value & 0xffu));
}

inline void appendVarLen(std::vector<uint8_t>& data, uint32_t value)
{
    uint8_t buffer[5] {};
    int count = 0;
    buffer[count++] = static_cast<uint8_t>(value & 0x7fu);
    while ((value >>= 7u) != 0u)
        buffer[count++] = static_cast<uint8_t>((value & 0x7fu) | 0x80u);
    while (count > 0)
        data.push_back(buffer[--count]);
}

inline bool writeMidiFile(const std::string& path, const MelodyPattern& pattern,
                          const float rate, const float swing)
{
    std::vector<MidiFileEvent> events;
    events.reserve(pattern.length * 2u);

    uint32_t tick = 0u;
    for (uint32_t i = 0u; i < pattern.length; ++i)
    {
        const uint32_t stepTicks = stepDurationTicks(kMidiPpq, rate, swing, i);
        const MelodyStep& step = pattern.steps[i];
        if (step.active != 0u)
        {
            const uint32_t gateTicks = std::max<uint32_t>(
                1u, static_cast<uint32_t>(std::lround(
                    static_cast<double>(stepTicks) * static_cast<double>(step.gate) / 127.0)));
            events.push_back({tick, 0x90u, static_cast<uint8_t>(clampi(step.note, 0, 127)), step.velocity});
            events.push_back({tick + gateTicks, 0x80u, static_cast<uint8_t>(clampi(step.note, 0, 127)), 0u});
        }
        tick += stepTicks;
    }

    std::sort(events.begin(), events.end(), [](const MidiFileEvent& a, const MidiFileEvent& b) {
        if (a.tick != b.tick) return a.tick < b.tick;
        return a.status < b.status;
    });

    std::vector<uint8_t> track;
    const char* const trackName = "Maker Trance Melody";
    appendVarLen(track, 0u);
    track.push_back(0xffu); track.push_back(0x03u);
    track.push_back(static_cast<uint8_t>(std::strlen(trackName)));
    track.insert(track.end(), trackName, trackName + std::strlen(trackName));

    appendVarLen(track, 0u);
    track.push_back(0xffu); track.push_back(0x58u); track.push_back(0x04u);
    track.push_back(0x04u); track.push_back(0x02u); track.push_back(0x18u); track.push_back(0x08u);

    uint32_t lastTick = 0u;
    for (const MidiFileEvent& event : events)
    {
        appendVarLen(track, event.tick - lastTick);
        track.push_back(event.status);
        track.push_back(event.note);
        track.push_back(event.velocity);
        lastTick = event.tick;
    }

    appendVarLen(track, tick > lastTick ? tick - lastTick : 0u);
    track.push_back(0xffu); track.push_back(0x2fu); track.push_back(0x00u);

    std::vector<uint8_t> file;
    file.reserve(track.size() + 22u);
    file.insert(file.end(), {'M','T','h','d'});
    appendBigEndian32(file, 6u);
    appendBigEndian16(file, 0u);
    appendBigEndian16(file, 1u);
    appendBigEndian16(file, static_cast<uint16_t>(kMidiPpq));
    file.insert(file.end(), {'M','T','r','k'});
    appendBigEndian32(file, static_cast<uint32_t>(track.size()));
    file.insert(file.end(), track.begin(), track.end());

    std::ofstream output(path.c_str(), std::ios::binary | std::ios::trunc);
    if (!output) return false;
    output.write(reinterpret_cast<const char*>(file.data()), static_cast<std::streamsize>(file.size()));
    return output.good();
}

} // namespace MakerTrance
