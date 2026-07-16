#include "DistrhoUI.hpp"
#include "Color.hpp"
#include "MakerTranceShared.hpp"

#include <array>
#include <cmath>
#include <cstdio>
#include <cstring>

START_NAMESPACE_DISTRHO

using DGL_NAMESPACE::Color;
using DGL_NAMESPACE::GraphicsContext;
using DGL_NAMESPACE::Rectangle;
using namespace MakerTrance;

namespace
{
struct UiRect
{
    double x, y, w, h;
    bool contains(const double px, const double py) const noexcept
    {
        return px >= x && py >= y && px <= x + w && py <= y + h;
    }
};

struct Control
{
    uint32_t parameter;
    UiRect rect;
    const char* label;
};

constexpr std::array<Control, 26> kControls {{
    {pRoot,          {24, 250, 132, 48}, "ROOT NOTE"},
    {pScale,         {166, 250, 132, 48}, "SCALE"},
    {pRate,          {24, 306, 132, 48}, "RATE"},
    {pLength,        {166, 306, 132, 48}, "LENGTH"},
    {pDensity,       {24, 362, 132, 48}, "DENSITY"},
    {pOctaves,       {166, 362, 132, 48}, "OCTAVES"},
    {pMotion,        {24, 418, 132, 48}, "MOTION"},
    {pGateLength,    {166, 418, 132, 48}, "NOTE GATE"},
    {pSwing,         {24, 474, 132, 48}, "SWING"},
    {pHumanize,      {166, 474, 132, 48}, "HUMANIZE"},

    {pUnison,        {326, 330, 103, 48}, "UNISON"},
    {pDetune,        {437, 330, 103, 48}, "DETUNE"},
    {pSpread,        {548, 330, 103, 48}, "WIDTH"},
    {pColor,         {659, 330, 103, 48}, "COLOR"},
    {pOsc2Mix,       {770, 330, 103, 48}, "OSC 2"},
    {pSub,           {881, 330, 95, 48},  "SUB"},

    {pDrive,         {326, 386, 103, 48}, "DRIVE"},
    {pCutoff,        {437, 386, 103, 48}, "CUTOFF"},
    {pResonance,     {548, 386, 103, 48}, "RESONANCE"},
    {pFilterEnv,     {659, 386, 103, 48}, "FILTER ENV"},
    {pFilterD,       {770, 386, 103, 48}, "FILTER DEC"},
    {pVibrato,       {881, 386, 95, 48},  "VIBRATO"},

    {pGateDepth,     {326, 476, 103, 48}, "TRANCE GATE"},
    {pDelayMix,      {437, 476, 103, 48}, "DELAY"},
    {pReverbMix,     {548, 476, 103, 48}, "REVERB"},
    {pChorus,        {659, 476, 103, 48}, "CHORUS"}
}};

constexpr UiRect kManualButton {24, 105, 130, 38};
constexpr UiRect kAutoButton   {166, 105, 132, 38};
constexpr UiRect kUpliftButton {24, 157, 84, 38};
constexpr UiRect kPsyButton    {116, 157, 84, 38};
constexpr UiRect kProgButton   {208, 157, 90, 38};
constexpr UiRect kGenerateButton {24, 205, 274, 38};
constexpr UiRect kMidiButton {800, 542, 176, 48};
constexpr UiRect kMasterControl {326, 542, 454, 48};

const std::array<uint8_t, 7>& glyphRows(const char c)
{
    static const std::array<uint8_t, 7> blank {{0,0,0,0,0,0,0}};
#define G(name,a,b,c,d,e,f,g) static const std::array<uint8_t,7> name {{a,b,c,d,e,f,g}}
    G(A,14,17,17,31,17,17,17); G(B,30,17,17,30,17,17,30);
    G(C,14,17,16,16,16,17,14); G(D,30,17,17,17,17,17,30);
    G(E,31,16,16,30,16,16,31); G(F,31,16,16,30,16,16,16);
    G(G,14,17,16,23,17,17,15); G(H,17,17,17,31,17,17,17);
    G(I,31,4,4,4,4,4,31); G(J,7,2,2,2,18,18,12);
    G(K,17,18,20,24,20,18,17); G(L,16,16,16,16,16,16,31);
    G(M,17,27,21,21,17,17,17); G(N,17,25,21,19,17,17,17);
    G(O,14,17,17,17,17,17,14); G(P,30,17,17,30,16,16,16);
    G(Q,14,17,17,17,21,18,13); G(R,30,17,17,30,20,18,17);
    G(S,15,16,16,14,1,1,30); G(T,31,4,4,4,4,4,4);
    G(U,17,17,17,17,17,17,14); G(V,17,17,17,17,17,10,4);
    G(W,17,17,17,21,21,21,10); G(X,17,17,10,4,10,17,17);
    G(Y,17,17,10,4,4,4,4); G(Z,31,1,2,4,8,16,31);
    G(N0,14,17,19,21,25,17,14); G(N1,4,12,4,4,4,4,14);
    G(N2,14,17,1,2,4,8,31); G(N3,30,1,1,14,1,1,30);
    G(N4,2,6,10,18,31,2,2); G(N5,31,16,16,30,1,1,30);
    G(N6,14,16,16,30,17,17,14); G(N7,31,1,2,4,8,8,8);
    G(N8,14,17,17,14,17,17,14); G(N9,14,17,17,15,1,1,14);
    G(DASH,0,0,0,31,0,0,0); G(DOT,0,0,0,0,0,12,12);
    G(HASH,10,31,10,10,31,10,0); G(SLASH,1,2,4,8,16,0,0);
    G(COLON,0,12,12,0,12,12,0); G(PERCENT,17,2,4,8,16,17,0);
#undef G
    switch (c)
    {
    case 'A': return A; case 'B': return B; case 'C': return C; case 'D': return D;
    case 'E': return E; case 'F': return F; case 'G': return G; case 'H': return H;
    case 'I': return I; case 'J': return J; case 'K': return K; case 'L': return L;
    case 'M': return M; case 'N': return N; case 'O': return O; case 'P': return P;
    case 'Q': return Q; case 'R': return R; case 'S': return S; case 'T': return T;
    case 'U': return U; case 'V': return V; case 'W': return W; case 'X': return X;
    case 'Y': return Y; case 'Z': return Z;
    case '0': return N0; case '1': return N1; case '2': return N2; case '3': return N3;
    case '4': return N4; case '5': return N5; case '6': return N6; case '7': return N7;
    case '8': return N8; case '9': return N9;
    case '-': return DASH; case '.': return DOT; case '#': return HASH;
    case '/': return SLASH; case ':': return COLON; case '%': return PERCENT;
    default: return blank;
    }
}

} // namespace

class MakerTranceUI final : public UI
{
public:
    MakerTranceUI()
        : UI(DISTRHO_UI_DEFAULT_WIDTH, DISTRHO_UI_DEFAULT_HEIGHT)
    {
        for (uint32_t i = 0; i < kParameterCount; ++i)
            fParameters[i] = kParameterSpecs[i].def;
        rebuildPattern();
    }

protected:
    void parameterChanged(const uint32_t index, const float value) override
    {
        if (index >= kParameterCount) return;
        fParameters[index] = value;
        if ((index >= pStyle && index <= pHumanize) || index == pSeedLow || index == pSeedHigh)
            rebuildPattern();
        repaint();
    }

    void programLoaded(const uint32_t) override
    {
        rebuildPattern();
        repaint();
    }

    void onDisplay() override
    {
        const GraphicsContext& context(getGraphicsContext());
        drawRect(context, {0, 0, 1000, 620}, Color(0.012f, 0.013f, 0.017f));
        drawRect(context, {0, 0, 1000, 5}, Color(0.76f, 0.015f, 0.025f));

        // Subtle red grid and glow strips.
        for (int x = 0; x < 1000; x += 40)
            drawRect(context, {static_cast<double>(x), 0, 1, 620}, Color(0.12f, 0.018f, 0.024f, 0.28f));
        for (int y = 0; y < 620; y += 40)
            drawRect(context, {0, static_cast<double>(y), 1000, 1}, Color(0.12f, 0.018f, 0.024f, 0.20f));

        drawRect(context, {14, 14, 972, 62}, Color(0.025f, 0.026f, 0.034f));
        drawBorder(context, {14, 14, 972, 62}, Color(0.62f, 0.018f, 0.028f), 2.0);
        drawText(context, 28, 27, "MAKER TRANCE", 3.0, Color(0.96f, 0.08f, 0.10f));
        drawText(context, 28, 55, "BY ALZA PRODUZ", 1.5, Color(0.70f, 0.72f, 0.76f));
        drawText(context, 704, 31, "VST2  X64", 1.8, Color(0.88f, 0.88f, 0.91f));
        drawText(context, 704, 54, "TRANCE MELODY SYNTH", 1.25, Color(0.60f, 0.62f, 0.67f));

        drawPanel(context, {14, 88, 296, 458}, "MELODY ENGINE");
        drawPanel(context, {316, 88, 670, 218}, "GENERATED SEQUENCE");
        drawPanel(context, {316, 318, 670, 126}, "SOUND ENGINE");
        drawPanel(context, {316, 454, 670, 82}, "TRANCE FX");

        drawButton(context, kManualButton, "MANUAL", fParameters[pMode] < 0.5f);
        drawButton(context, kAutoButton, "AUTO", fParameters[pMode] >= 0.5f);
        drawButton(context, kUpliftButton, "UPLIFT", std::lround(fParameters[pStyle]) == 0);
        drawButton(context, kPsyButton, "PSY", std::lround(fParameters[pStyle]) == 1);
        drawButton(context, kProgButton, "PROG", std::lround(fParameters[pStyle]) == 2);
        drawButton(context, kGenerateButton, "GENERATE NEW MELODY", true, true);

        for (const Control& control : kControls)
            drawControl(context, control);

        drawSequence(context);

        drawRect(context, {14, 556, 296, 48}, Color(0.024f, 0.025f, 0.032f));
        drawBorder(context, {14, 556, 296, 48}, Color(0.26f, 0.035f, 0.045f), 1.0);
        char generationText[64];
        const uint32_t generation = generationFromValues(fParameters[pSeedLow], fParameters[pSeedHigh]);
        std::snprintf(generationText, sizeof(generationText), "GENERATION %u", generation);
        drawText(context, 27, 568, generationText, 1.35, Color(0.94f, 0.14f, 0.16f));
        drawText(context, 27, 588, "4.29 BILLION UNIQUE EVENT PATTERNS", 0.92, Color(0.56f, 0.58f, 0.62f));

        drawMaster(context);
        drawButton(context, kMidiButton, "MIDI OUT", fParameters[pMidiOut] >= 0.5f);
        drawText(context, 326, 602, "DRAG BARS OR CLICK POSITION   AUTO MODE RUNS WITH FL TRANSPORT", 1.05,
                 Color(0.46f, 0.48f, 0.52f));
    }

    bool onMouse(const MouseEvent& ev) override
    {
        const double x = ev.pos.getX();
        const double y = ev.pos.getY();

        if (ev.button != 1) return false;
        if (!ev.press)
        {
            if (fActiveParameter >= 0)
            {
                editParameter(static_cast<uint32_t>(fActiveParameter), false);
                fActiveParameter = -1;
                return true;
            }
            return false;
        }

        if (kManualButton.contains(x, y)) return setDiscrete(pMode, 0.0f);
        if (kAutoButton.contains(x, y)) return setDiscrete(pMode, 1.0f);
        if (kUpliftButton.contains(x, y)) return setDiscrete(pStyle, 0.0f);
        if (kPsyButton.contains(x, y)) return setDiscrete(pStyle, 1.0f);
        if (kProgButton.contains(x, y)) return setDiscrete(pStyle, 2.0f);
        if (kMidiButton.contains(x, y)) return setDiscrete(pMidiOut, fParameters[pMidiOut] >= 0.5f ? 0.0f : 1.0f);
        if (kGenerateButton.contains(x, y))
        {
            generateNewMelody();
            return true;
        }
        if (kMasterControl.contains(x, y))
        {
            fActiveParameter = static_cast<int>(pMaster);
            editParameter(pMaster, true);
            setFromPosition(pMaster, kMasterControl, x);
            return true;
        }

        for (const Control& control : kControls)
        {
            if (control.rect.contains(x, y))
            {
                fActiveParameter = static_cast<int>(control.parameter);
                editParameter(control.parameter, true);
                setFromPosition(control.parameter, control.rect, x);
                return true;
            }
        }
        return false;
    }

    bool onMotion(const MotionEvent& ev) override
    {
        if (fActiveParameter < 0) return false;
        const uint32_t parameter = static_cast<uint32_t>(fActiveParameter);
        const double x = ev.pos.getX();
        if (parameter == pMaster)
            setFromPosition(parameter, kMasterControl, x);
        else
        {
            for (const Control& control : kControls)
            {
                if (control.parameter == parameter)
                {
                    setFromPosition(parameter, control.rect, x);
                    break;
                }
            }
        }
        return true;
    }

private:
    void rebuildPattern() noexcept
    {
        fPattern = generateMelody(configFromParameters(fParameters));
    }

    bool setDiscrete(const uint32_t parameter, const float value)
    {
        editParameter(parameter, true);
        fParameters[parameter] = value;
        setParameterValue(parameter, value);
        editParameter(parameter, false);
        if (parameter == pStyle) rebuildPattern();
        repaint();
        return true;
    }

    void setFromPosition(const uint32_t parameter, const UiRect& rect, const double x)
    {
        const float normalized = clampf(static_cast<float>((x - rect.x - 8.0) / std::max(1.0, rect.w - 16.0)), 0.0f, 1.0f);
        const float value = normalizedToValue(parameter, normalized);
        fParameters[parameter] = value;
        setParameterValue(parameter, value);
        if ((parameter >= pStyle && parameter <= pHumanize) || parameter == pSeedLow || parameter == pSeedHigh)
            rebuildPattern();
        repaint();
    }

    void generateNewMelody()
    {
        uint32_t generation = generationFromValues(fParameters[pSeedLow], fParameters[pSeedHigh]);
        ++generation;
        float low = 0.0f;
        float high = 0.0f;
        valuesFromGeneration(generation, low, high);
        editParameter(pSeedLow, true);
        fParameters[pSeedLow] = low;
        setParameterValue(pSeedLow, low);
        editParameter(pSeedLow, false);
        editParameter(pSeedHigh, true);
        fParameters[pSeedHigh] = high;
        setParameterValue(pSeedHigh, high);
        editParameter(pSeedHigh, false);
        rebuildPattern();
        repaint();
    }

    void drawRect(const GraphicsContext& context, const UiRect& r, const Color& color) const
    {
        color.setFor(context);
        Rectangle<double>(r.x, r.y, r.w, r.h).draw(context);
    }

    void drawBorder(const GraphicsContext& context, const UiRect& r, const Color& color, const double width) const
    {
        drawRect(context, {r.x, r.y, r.w, width}, color);
        drawRect(context, {r.x, r.y + r.h - width, r.w, width}, color);
        drawRect(context, {r.x, r.y, width, r.h}, color);
        drawRect(context, {r.x + r.w - width, r.y, width, r.h}, color);
    }

    void drawText(const GraphicsContext& context, double x, const double y, const char* text,
                  const double scale, const Color& color) const
    {
        color.setFor(context);
        const double pixel = std::max(0.72, scale);
        const double advance = pixel * 6.0;
        for (const char* p = text; *p != '\0'; ++p)
        {
            const char c = *p >= 'a' && *p <= 'z' ? static_cast<char>(*p - 32) : *p;
            if (c == ' ')
            {
                x += advance;
                continue;
            }
            const auto& rows = glyphRows(c);
            for (int row = 0; row < 7; ++row)
            {
                for (int col = 0; col < 5; ++col)
                {
                    if ((rows[static_cast<uint32_t>(row)] & (1u << (4 - col))) != 0u)
                        Rectangle<double>(x + col * pixel, y + row * pixel, pixel, pixel).draw(context);
                }
            }
            x += advance;
        }
    }

    void drawPanel(const GraphicsContext& context, const UiRect& r, const char* title) const
    {
        drawRect(context, r, Color(0.020f, 0.021f, 0.028f));
        drawBorder(context, r, Color(0.22f, 0.026f, 0.036f), 1.0);
        drawRect(context, {r.x, r.y, r.w, 28}, Color(0.032f, 0.027f, 0.035f));
        drawRect(context, {r.x, r.y + 27, r.w, 1}, Color(0.60f, 0.018f, 0.029f));
        drawText(context, r.x + 12, r.y + 9, title, 1.25, Color(0.84f, 0.84f, 0.87f));
    }

    void drawButton(const GraphicsContext& context, const UiRect& r, const char* label,
                    const bool active, const bool generator = false) const
    {
        const Color fill = active ? Color(0.46f, 0.012f, 0.022f) : Color(0.045f, 0.046f, 0.058f);
        drawRect(context, r, fill);
        drawBorder(context, r, active ? Color(0.96f, 0.055f, 0.073f) : Color(0.22f, 0.23f, 0.27f), active ? 2.0 : 1.0);
        if (generator)
            drawRect(context, {r.x + 4, r.y + 4, 4, r.h - 8}, Color(1.0f, 0.12f, 0.14f));
        const double scale = std::strlen(label) > 15 ? 1.15 : 1.35;
        const double textWidth = std::strlen(label) * scale * 6.0;
        drawText(context, r.x + (r.w - textWidth) * 0.5, r.y + (r.h - scale * 7.0) * 0.5,
                 label, scale, active ? Color(1.0f, 0.92f, 0.93f) : Color(0.67f, 0.68f, 0.72f));
    }

    void valueString(const uint32_t parameter, char* text, const size_t size) const
    {
        const float value = fParameters[parameter];
        switch (parameter)
        {
        case pRoot:
            std::snprintf(text, size, "%s%d", noteName(static_cast<int>(std::lround(value))),
                          static_cast<int>(std::lround(value)) / 12 - 1);
            break;
        case pScale:
            std::snprintf(text, size, "%s", scaleName(static_cast<int>(std::lround(value))));
            break;
        case pRate:
        {
            static constexpr const char* values[] = {"1/8", "1/16", "1/16T", "1/32"};
            std::snprintf(text, size, "%s", values[clampi(static_cast<int>(std::lround(value)), 0, 3)]);
            break;
        }
        case pLength:
            std::snprintf(text, size, "%u", melodyLengthFromParam(value));
            break;
        case pOctaves:
        case pUnison:
            std::snprintf(text, size, "%d", static_cast<int>(std::lround(value)));
            break;
        case pDetune:
            std::snprintf(text, size, "%d CT", static_cast<int>(std::lround(value)));
            break;
        case pCutoff:
            if (value >= 1000.0f) std::snprintf(text, size, "%.1fK", value / 1000.0f);
            else std::snprintf(text, size, "%d HZ", static_cast<int>(value));
            break;
        case pFilterEnv:
            std::snprintf(text, size, "%+.2f", value);
            break;
        case pDrive:
        case pFilterD:
            std::snprintf(text, size, "%.2f", value);
            break;
        default:
            std::snprintf(text, size, "%d%%", static_cast<int>(std::lround(valueToNormalized(parameter, value) * 100.0f)));
            break;
        }
    }

    void drawControl(const GraphicsContext& context, const Control& control) const
    {
        const float normalized = valueToNormalized(control.parameter, fParameters[control.parameter]);
        drawRect(context, control.rect, Color(0.032f, 0.033f, 0.042f));
        drawBorder(context, control.rect, Color(0.14f, 0.14f, 0.17f), 1.0);
        drawText(context, control.rect.x + 7, control.rect.y + 7, control.label, 0.86,
                 Color(0.64f, 0.65f, 0.69f));
        const UiRect track {control.rect.x + 7, control.rect.y + 29, control.rect.w - 14, 9};
        drawRect(context, track, Color(0.010f, 0.011f, 0.014f));
        drawRect(context, {track.x, track.y, track.w * normalized, track.h}, Color(0.74f, 0.018f, 0.030f));
        drawRect(context, {track.x + track.w * normalized - 1.0, track.y - 2, 3, track.h + 4},
                 Color(1.0f, 0.16f, 0.18f));
        char text[40];
        valueString(control.parameter, text, sizeof(text));
        const double tw = std::strlen(text) * 0.78 * 6.0;
        drawText(context, control.rect.x + control.rect.w - tw - 6, control.rect.y + 7, text, 0.78,
                 Color(0.94f, 0.90f, 0.91f));
    }

    void drawSequence(const GraphicsContext& context) const
    {
        const UiRect area {328, 126, 646, 164};
        drawRect(context, area, Color(0.010f, 0.011f, 0.015f));
        drawBorder(context, area, Color(0.22f, 0.025f, 0.035f), 1.0);
        for (int i = 1; i < 8; ++i)
            drawRect(context, {area.x, area.y + i * area.h / 8.0, area.w, 1}, Color(0.09f, 0.025f, 0.030f));

        const uint32_t length = fPattern.length;
        const double stepWidth = area.w / static_cast<double>(length);
        for (uint32_t i = 0; i < length; ++i)
        {
            const MelodyStep& step = fPattern.steps[i];
            const double x = area.x + i * stepWidth;
            if (i % 4u == 0u)
                drawRect(context, {x, area.y, 1.0, area.h}, Color(0.36f, 0.028f, 0.040f));
            else
                drawRect(context, {x, area.y, 1.0, area.h}, Color(0.10f, 0.022f, 0.029f));
            if (step.active == 0u) continue;
            const double noteNorm = clampf((static_cast<float>(step.note) - 24.0f) / 72.0f, 0.0f, 1.0f);
            const double y = area.y + area.h - 12.0 - noteNorm * (area.h - 24.0);
            const double velocity = static_cast<double>(step.velocity) / 127.0;
            drawRect(context, {x + 2.0, y, std::max(2.0, stepWidth - 4.0), 8.0},
                     Color(0.55f + static_cast<float>(velocity) * 0.35f, 0.015f, 0.030f));
            drawRect(context, {x + 2.0, y + 9.0, std::max(2.0, stepWidth - 4.0),
                               std::max(2.0, static_cast<double>(step.gate) / 18.0)},
                     Color(0.24f, 0.022f, 0.035f));
        }

        char status[128];
        std::snprintf(status, sizeof(status), "%s  %s  ROOT %s%d  %u STEPS",
                      styleName(static_cast<int>(std::lround(fParameters[pStyle]))),
                      scaleName(static_cast<int>(std::lround(fParameters[pScale]))),
                      noteName(static_cast<int>(std::lround(fParameters[pRoot]))),
                      static_cast<int>(std::lround(fParameters[pRoot])) / 12 - 1,
                      length);
        drawText(context, 334, 101, status, 1.05, Color(0.78f, 0.79f, 0.82f));
        drawText(context, 790, 101, fParameters[pMode] >= 0.5f ? "AUTO ACTIVE" : "MANUAL MIDI",
                 1.05, fParameters[pMode] >= 0.5f ? Color(0.96f, 0.08f, 0.10f) : Color(0.58f, 0.60f, 0.65f));
    }

    void drawMaster(const GraphicsContext& context) const
    {
        const float normalized = valueToNormalized(pMaster, fParameters[pMaster]);
        drawRect(context, kMasterControl, Color(0.032f, 0.033f, 0.042f));
        drawBorder(context, kMasterControl, Color(0.20f, 0.20f, 0.24f), 1.0);
        drawText(context, 338, 553, "MASTER OUTPUT", 1.15, Color(0.76f, 0.77f, 0.80f));
        const UiRect track {338, 574, 422, 9};
        drawRect(context, track, Color(0.009f, 0.010f, 0.013f));
        drawRect(context, {track.x, track.y, track.w * normalized, track.h}, Color(0.76f, 0.018f, 0.030f));
        drawRect(context, {track.x + track.w * normalized - 1.0, track.y - 2, 3, track.h + 4}, Color(1.0f, 0.15f, 0.17f));
        char text[20];
        std::snprintf(text, sizeof(text), "%d%%", static_cast<int>(normalized * 100.0f));
        drawText(context, 716, 553, text, 1.05, Color(0.96f, 0.91f, 0.92f));
    }

    std::array<float, kParameterCount> fParameters {};
    MelodyPattern fPattern {};
    int fActiveParameter = -1;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MakerTranceUI)
};

UI* createUI()
{
    return new MakerTranceUI();
}

END_NAMESPACE_DISTRHO
