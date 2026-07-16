#include "DistrhoUI.hpp"
#include "Color.hpp"
#include "MakerTranceShared.hpp"
#include "MakerTranceMidi.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>

#ifdef _WIN32
# include <windows.h>
# include <shellapi.h>
# include <shlobj.h>
#endif

START_NAMESPACE_DISTRHO

using DGL_NAMESPACE::Color;
using DGL_NAMESPACE::GraphicsContext;
using DGL_NAMESPACE::Rectangle;
using namespace MakerTrance;

namespace
{
constexpr double kDesignWidth = 1100.0;
constexpr double kDesignHeight = 680.0;
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

constexpr UiRect kTabMelody {24, 84, 154, 38};
constexpr UiRect kTabSynth {188, 84, 154, 38};
constexpr UiRect kTabFx {352, 84, 154, 38};
constexpr UiRect kSizeSmall {900, 24, 48, 34};
constexpr UiRect kSizeMedium {956, 24, 48, 34};
constexpr UiRect kSizeLarge {1012, 24, 64, 34};

constexpr UiRect kUpliftButton {24, 150, 100, 42};
constexpr UiRect kPsyButton {134, 150, 100, 42};
constexpr UiRect kProgButton {244, 150, 100, 42};
constexpr UiRect kGenerateButton {364, 146, 190, 50};
constexpr UiRect kPreviewButton {564, 146, 174, 50};
constexpr UiRect kStopButton {748, 146, 112, 50};
constexpr UiRect kExportButton {870, 146, 206, 50};
constexpr UiRect kMasterControl {840, 624, 236, 38};

constexpr std::array<Control, 10> kMelodyControls {{
    {pRoot,       {24, 462, 196, 58}, "NOTA RAIZ"},
    {pScale,      {230, 462, 196, 58}, "ESCALA"},
    {pRate,       {436, 462, 196, 58}, "DIVISAO"},
    {pLength,     {642, 462, 196, 58}, "PASSOS"},
    {pDensity,    {848, 462, 228, 58}, "DENSIDADE"},
    {pOctaves,    {24, 530, 196, 58}, "OITAVAS"},
    {pMotion,     {230, 530, 196, 58}, "MOVIMENTO"},
    {pGateLength, {436, 530, 196, 58}, "DURACAO"},
    {pSwing,      {642, 530, 196, 58}, "SWING"},
    {pHumanize,   {848, 530, 228, 58}, "HUMANIZAR"}
}};

constexpr std::array<Control, 18> kSynthControls {{
    {pUnison,     {24, 156, 166, 66}, "UNISON"},
    {pDetune,     {200, 156, 166, 66}, "DETUNE"},
    {pSpread,     {376, 156, 166, 66}, "ABERTURA"},
    {pColor,      {552, 156, 166, 66}, "TIMBRE"},
    {pOsc2Mix,    {728, 156, 166, 66}, "OSC 2 MIX"},
    {pOsc2Semi,   {904, 156, 172, 66}, "OSC 2 SEMI"},

    {pSub,        {24, 238, 166, 66}, "SUB"},
    {pNoise,      {200, 238, 166, 66}, "AR NOISE"},
    {pDrive,      {376, 238, 166, 66}, "DRIVE"},
    {pCutoff,     {552, 238, 166, 66}, "CUTOFF"},
    {pResonance,  {728, 238, 166, 66}, "RESSONANCIA"},
    {pFilterEnv,  {904, 238, 172, 66}, "FILTRO ENV"},

    {pFilterD,    {24, 320, 166, 66}, "FILTRO DECAY"},
    {pVibrato,    {200, 320, 166, 66}, "VIBRATO"},
    {pAmpA,       {376, 320, 166, 66}, "AMP ATTACK"},
    {pAmpD,       {552, 320, 166, 66}, "AMP DECAY"},
    {pAmpS,       {728, 320, 166, 66}, "AMP SUSTAIN"},
    {pAmpR,       {904, 320, 172, 66}, "AMP RELEASE"}
}};

constexpr std::array<Control, 8> kFxControls {{
    {pGateDepth,     {24, 170, 244, 70}, "TRANCE GATE"},
    {pDelayMix,      {278, 170, 244, 70}, "DELAY MIX"},
    {pDelayFeedback, {532, 170, 244, 70}, "DELAY FEEDBACK"},
    {pDelayDivision, {786, 170, 290, 70}, "DELAY DIVISAO"},
    {pReverbMix,     {24, 260, 244, 70}, "REVERB MIX"},
    {pReverbSize,    {278, 260, 244, 70}, "REVERB TAMANHO"},
    {pReverbDamp,    {532, 260, 244, 70}, "REVERB DAMP"},
    {pChorus,        {786, 260, 290, 70}, "CHORUS"}
}};

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
    G(PLUS,0,4,4,31,4,4,0); G(EQUAL,0,31,0,31,0,0,0);
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
    case '+': return PLUS; case '=': return EQUAL;
    default: return blank;
    }
}

std::string midiExportPath(const uint32_t generation)
{
#ifdef _WIN32
    char documents[MAX_PATH] {};
    if (SHGetFolderPathA(nullptr, CSIDL_PERSONAL | CSIDL_FLAG_CREATE,
                         nullptr, SHGFP_TYPE_CURRENT, documents) != S_OK)
    {
        const DWORD length = GetEnvironmentVariableA("USERPROFILE", documents, MAX_PATH);
        if (length == 0u || length >= MAX_PATH)
            std::snprintf(documents, sizeof(documents), ".");
        else
            std::strncat(documents, "\\Documents", sizeof(documents) - std::strlen(documents) - 1u);
    }
    std::string folder(documents);
    folder += "\\Maker Trance";
    CreateDirectoryA(folder.c_str(), nullptr);
    return folder + "\\Maker-Trance-Melody-" + std::to_string(generation) + ".mid";
#else
    return std::string("Maker-Trance-Melody-") + std::to_string(generation) + ".mid";
#endif
}

void revealMidiFile(const std::string& path)
{
#ifdef _WIN32
    const std::string arguments = std::string("/select,\"") + path + "\"";
    ShellExecuteA(nullptr, "open", "explorer.exe", arguments.c_str(), nullptr, SW_SHOWNORMAL);
#else
    (void)path;
#endif
}

} // namespace

class MakerTranceUI final : public UI
{
public:
    MakerTranceUI()
        : UI(DISTRHO_UI_DEFAULT_WIDTH, DISTRHO_UI_DEFAULT_HEIGHT)
    {
        setGeometryConstraints(760u, 470u, true);
        for (uint32_t i = 0; i < kParameterCount; ++i)
            fParameters[i] = kParameterSpecs[i].def;
        std::snprintf(fStatus, sizeof(fStatus), "GERE UMA MELODIA, OUCA A PREVIA E EXPORTE PARA O PIANO ROLL.");
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

    void onResize(const ResizeEvent&) override
    {
        repaint();
    }

    void onDisplay() override
    {
        updateTransform();
        const GraphicsContext& context(getGraphicsContext());

        Color background(0.010f, 0.011f, 0.015f);
        background.setFor(context);
        Rectangle<double>(0.0, 0.0, static_cast<double>(getWidth()), static_cast<double>(getHeight())).draw(context);

        drawRect(context, {0, 0, kDesignWidth, 6}, Color(0.85f, 0.012f, 0.025f));
        for (int x = 0; x < static_cast<int>(kDesignWidth); x += 52)
            drawRect(context, {static_cast<double>(x), 0, 1, kDesignHeight}, Color(0.12f, 0.018f, 0.024f, 0.22f));
        for (int y = 0; y < static_cast<int>(kDesignHeight); y += 52)
            drawRect(context, {0, static_cast<double>(y), kDesignWidth, 1}, Color(0.12f, 0.018f, 0.024f, 0.16f));

        drawRect(context, {18, 18, 1064, 52}, Color(0.024f, 0.025f, 0.033f));
        drawBorder(context, {18, 18, 1064, 52}, Color(0.70f, 0.018f, 0.030f), 2.0);
        drawText(context, 34, 28, "MAKER TRANCE", 3.0, Color(0.98f, 0.075f, 0.095f));
        drawText(context, 36, 54, "BY ALZA PRODUZ  VST2 X64", 1.12, Color(0.74f, 0.75f, 0.79f));
        drawText(context, 754, 35, "TAMANHO", 1.10, Color(0.72f, 0.73f, 0.77f));
        drawButton(context, kSizeSmall, "S", false);
        drawButton(context, kSizeMedium, "M", false);
        drawButton(context, kSizeLarge, "L", false);

        drawButton(context, kTabMelody, "MELODIA", fPage == 0);
        drawButton(context, kTabSynth, "SINTETIZADOR", fPage == 1);
        drawButton(context, kTabFx, "EFEITOS", fPage == 2);

        drawPanel(context, {18, 132, 1064, 474}, fPage == 0 ? "GERADOR E PIANO ROLL" : (fPage == 1 ? "MOTOR SONORO" : "EFEITOS DE TRANCE"));

        if (fPage == 0)
            drawMelodyPage(context);
        else if (fPage == 1)
            drawSynthPage(context);
        else
            drawFxPage(context);

        drawStatusAndMaster(context);
    }

    bool onMouse(const MouseEvent& ev) override
    {
        updateTransform();
        const double x = (ev.pos.getX() - fOffsetX) / fScale;
        const double y = (ev.pos.getY() - fOffsetY) / fScale;

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

        if (kTabMelody.contains(x, y)) { fPage = 0; repaint(); return true; }
        if (kTabSynth.contains(x, y)) { fPage = 1; repaint(); return true; }
        if (kTabFx.contains(x, y)) { fPage = 2; repaint(); return true; }
        if (kSizeSmall.contains(x, y)) { setSize(820u, 507u); return true; }
        if (kSizeMedium.contains(x, y)) { setSize(970u, 600u); return true; }
        if (kSizeLarge.contains(x, y)) { setSize(1100u, 680u); return true; }

        if (fPage == 0)
        {
            if (kUpliftButton.contains(x, y)) return setDiscrete(pStyle, 0.0f);
            if (kPsyButton.contains(x, y)) return setDiscrete(pStyle, 1.0f);
            if (kProgButton.contains(x, y)) return setDiscrete(pStyle, 2.0f);
            if (kGenerateButton.contains(x, y)) { generateNewMelody(); return true; }
            if (kPreviewButton.contains(x, y)) { triggerPreview(); return true; }
            if (kStopButton.contains(x, y)) { triggerStop(); return true; }
            if (kExportButton.contains(x, y)) { exportMidi(); return true; }
        }

        if (kMasterControl.contains(x, y))
        {
            beginControl(pMaster, kMasterControl, x);
            return true;
        }

        const Control* const control = findControlAt(x, y);
        if (control != nullptr)
        {
            beginControl(control->parameter, control->rect, x);
            return true;
        }
        return false;
    }

    bool onMotion(const MotionEvent& ev) override
    {
        if (fActiveParameter < 0) return false;
        updateTransform();
        const double x = (ev.pos.getX() - fOffsetX) / fScale;
        const uint32_t parameter = static_cast<uint32_t>(fActiveParameter);
        if (parameter == pMaster)
            setFromPosition(parameter, kMasterControl, x);
        else
        {
            const Control* const control = findControlByParameter(parameter);
            if (control != nullptr)
                setFromPosition(parameter, control->rect, x);
        }
        return true;
    }

private:
    void updateTransform() noexcept
    {
        const double scaleX = static_cast<double>(getWidth()) / kDesignWidth;
        const double scaleY = static_cast<double>(getHeight()) / kDesignHeight;
        fScale = std::max(0.01, std::min(scaleX, scaleY));
        fOffsetX = (static_cast<double>(getWidth()) - kDesignWidth * fScale) * 0.5;
        fOffsetY = (static_cast<double>(getHeight()) - kDesignHeight * fScale) * 0.5;
    }

    UiRect transform(const UiRect& r) const noexcept
    {
        return {fOffsetX + r.x * fScale, fOffsetY + r.y * fScale, r.w * fScale, r.h * fScale};
    }

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

    void beginControl(const uint32_t parameter, const UiRect& rect, const double x)
    {
        fActiveParameter = static_cast<int>(parameter);
        editParameter(parameter, true);
        setFromPosition(parameter, rect, x);
    }

    void setFromPosition(const uint32_t parameter, const UiRect& rect, const double x)
    {
        const float normalized = clampf(
            static_cast<float>((x - rect.x - 9.0) / std::max(1.0, rect.w - 18.0)), 0.0f, 1.0f);
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
        std::snprintf(fStatus, sizeof(fStatus), "MELODIA %u GERADA. CLIQUE TOCAR PREVIA OU EXPORTAR MIDI.", generation);
        repaint();
    }

    void incrementTrigger(const uint32_t parameter)
    {
        uint32_t value = static_cast<uint32_t>(clampi(static_cast<int>(std::lround(fParameters[parameter])), 0, 65535));
        value = (value + 1u) & 0xffffu;
        editParameter(parameter, true);
        fParameters[parameter] = static_cast<float>(value);
        setParameterValue(parameter, static_cast<float>(value));
        editParameter(parameter, false);
    }

    void triggerPreview()
    {
        incrementTrigger(pPreviewTrigger);
        std::snprintf(fStatus, sizeof(fStatus), "PREVIA TOCANDO UMA VEZ NO BPM ATUAL DO FL STUDIO.");
        repaint();
    }

    void triggerStop()
    {
        incrementTrigger(pStopTrigger);
        std::snprintf(fStatus, sizeof(fStatus), "PREVIA PARADA.");
        repaint();
    }

    void exportMidi()
    {
        const uint32_t generation = generationFromValues(fParameters[pSeedLow], fParameters[pSeedHigh]);
        const std::string path = midiExportPath(generation);
        if (writeMidiFile(path, fPattern, fParameters[pRate], fParameters[pSwing]))
        {
            fLastMidiPath = path;
            std::snprintf(fStatus, sizeof(fStatus), "MIDI SALVO. ARRASTE O ARQUIVO ABERTO PARA O PIANO ROLL.");
            revealMidiFile(path);
        }
        else
        {
            std::snprintf(fStatus, sizeof(fStatus), "ERRO AO SALVAR MIDI. VERIFIQUE A PASTA DOCUMENTOS.");
        }
        repaint();
    }

    const Control* findControlAt(const double x, const double y) const noexcept
    {
        if (fPage == 0)
        {
            for (const Control& c : kMelodyControls) if (c.rect.contains(x, y)) return &c;
        }
        else if (fPage == 1)
        {
            for (const Control& c : kSynthControls) if (c.rect.contains(x, y)) return &c;
        }
        else
        {
            for (const Control& c : kFxControls) if (c.rect.contains(x, y)) return &c;
        }
        return nullptr;
    }

    const Control* findControlByParameter(const uint32_t parameter) const noexcept
    {
        for (const Control& c : kMelodyControls) if (c.parameter == parameter) return &c;
        for (const Control& c : kSynthControls) if (c.parameter == parameter) return &c;
        for (const Control& c : kFxControls) if (c.parameter == parameter) return &c;
        return nullptr;
    }

    double textWidth(const char* text, const double scale) const noexcept
    {
        return static_cast<double>(std::strlen(text)) * std::max(0.78, scale) * 6.0;
    }

    void drawRect(const GraphicsContext& context, const UiRect& r, const Color& color) const
    {
        Color drawColor(color);
        drawColor.setFor(context);
        const UiRect sr = transform(r);
        Rectangle<double>(sr.x, sr.y, sr.w, sr.h).draw(context);
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
        Color drawColor(color);
        drawColor.setFor(context);
        const double virtualPixel = std::max(0.78, scale);
        const double pixel = virtualPixel * fScale;
        const double advance = virtualPixel * 6.0 * fScale;
        double screenX = fOffsetX + x * fScale;
        const double screenY = fOffsetY + y * fScale;
        for (const char* p = text; *p != '\0'; ++p)
        {
            const char c = *p >= 'a' && *p <= 'z' ? static_cast<char>(*p - 32) : *p;
            if (c == ' ')
            {
                screenX += advance;
                continue;
            }
            const auto& rows = glyphRows(c);
            for (int row = 0; row < 7; ++row)
            {
                for (int col = 0; col < 5; ++col)
                {
                    if ((rows[static_cast<uint32_t>(row)] & (1u << (4 - col))) != 0u)
                        Rectangle<double>(screenX + col * pixel, screenY + row * pixel, pixel, pixel).draw(context);
                }
            }
            screenX += advance;
        }
    }

    void drawPanel(const GraphicsContext& context, const UiRect& r, const char* title) const
    {
        drawRect(context, r, Color(0.019f, 0.020f, 0.027f));
        drawBorder(context, r, Color(0.28f, 0.026f, 0.040f), 1.0);
        drawRect(context, {r.x, r.y, r.w, 32}, Color(0.034f, 0.028f, 0.037f));
        drawRect(context, {r.x, r.y + 31, r.w, 1}, Color(0.67f, 0.018f, 0.031f));
        drawText(context, r.x + 14, r.y + 10, title, 1.48, Color(0.90f, 0.90f, 0.93f));
    }

    void drawButton(const GraphicsContext& context, const UiRect& r, const char* label, const bool active) const
    {
        const Color fill = active ? Color(0.50f, 0.012f, 0.025f) : Color(0.047f, 0.047f, 0.059f);
        drawRect(context, r, fill);
        drawBorder(context, r, active ? Color(1.0f, 0.055f, 0.080f) : Color(0.25f, 0.26f, 0.30f), active ? 2.0 : 1.0);
        double scale = 1.54;
        if (std::strlen(label) > 15u) scale = 1.23;
        else if (std::strlen(label) > 10u) scale = 1.36;
        const double tw = textWidth(label, scale);
        drawText(context, r.x + (r.w - tw) * 0.5, r.y + (r.h - scale * 7.0) * 0.5,
                 label, scale, active ? Color(1.0f, 0.95f, 0.96f) : Color(0.80f, 0.81f, 0.84f));
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
        case pOsc2Semi:
            std::snprintf(text, size, "%+d ST", static_cast<int>(std::lround(value)));
            break;
        case pCutoff:
            if (value >= 1000.0f) std::snprintf(text, size, "%.1fK", value / 1000.0f);
            else std::snprintf(text, size, "%d HZ", static_cast<int>(value));
            break;
        case pDelayDivision:
        {
            static constexpr const char* values[] = {"1/16", "1/8D", "1/8", "1/4D", "1/4"};
            std::snprintf(text, size, "%s", values[clampi(static_cast<int>(std::lround(value)), 0, 4)]);
            break;
        }
        case pFilterEnv:
            std::snprintf(text, size, "%+.2f", value);
            break;
        case pDrive:
        case pFilterD:
        case pAmpA:
        case pAmpD:
        case pAmpR:
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
        drawRect(context, control.rect, Color(0.031f, 0.032f, 0.041f));
        drawBorder(context, control.rect, Color(0.17f, 0.17f, 0.20f), 1.0);
        drawText(context, control.rect.x + 8, control.rect.y + 7, control.label, 1.20, Color(0.78f, 0.79f, 0.83f));

        const UiRect track {control.rect.x + 8, control.rect.y + 30, control.rect.w - 16, 10};
        drawRect(context, track, Color(0.008f, 0.009f, 0.012f));
        drawRect(context, {track.x, track.y, track.w * normalized, track.h}, Color(0.80f, 0.018f, 0.035f));
        drawRect(context, {track.x + track.w * normalized - 1.0, track.y - 2, 3, track.h + 4}, Color(1.0f, 0.16f, 0.19f));

        char text[40];
        valueString(control.parameter, text, sizeof(text));
        const double tw = textWidth(text, 1.10);
        drawText(context, control.rect.x + control.rect.w - tw - 8,
                 control.rect.y + control.rect.h - 15, text, 1.10, Color(0.98f, 0.94f, 0.95f));
    }

    void drawMelodyPage(const GraphicsContext& context) const
    {
        drawText(context, 24, 134, "ESTILO", 1.14, Color(0.75f, 0.76f, 0.80f));
        drawButton(context, kUpliftButton, "UPLIFT", std::lround(fParameters[pStyle]) == 0);
        drawButton(context, kPsyButton, "PSY", std::lround(fParameters[pStyle]) == 1);
        drawButton(context, kProgButton, "PROG", std::lround(fParameters[pStyle]) == 2);
        drawButton(context, kGenerateButton, "GERAR MELODIA", true);
        drawButton(context, kPreviewButton, "TOCAR PREVIA", true);
        drawButton(context, kStopButton, "PARAR", false);
        drawButton(context, kExportButton, "EXPORTAR MIDI", true);

        drawSequence(context);
        for (const Control& control : kMelodyControls) drawControl(context, control);
    }

    void drawSynthPage(const GraphicsContext& context) const
    {
        drawText(context, 30, 142, "CONTROLES DO SOM. TOQUE PELO PIANO ROLL OU USE A PREVIA NA ABA MELODIA.", 1.18,
                 Color(0.75f, 0.76f, 0.80f));
        for (const Control& control : kSynthControls) drawControl(context, control);

        drawRect(context, {24, 414, 1052, 158}, Color(0.026f, 0.027f, 0.035f));
        drawBorder(context, {24, 414, 1052, 158}, Color(0.24f, 0.027f, 0.040f), 1.0);
        drawText(context, 44, 438, "UPLIFTING", 1.70, Color(1.0f, 0.10f, 0.13f));
        drawText(context, 44, 464, "SUPERSAW ABERTA, BRILHO, EMOCAO E CAMADAS ESPIRITUAIS.", 1.18, Color(0.78f, 0.79f, 0.83f));
        drawText(context, 44, 500, "PSY", 1.70, Color(1.0f, 0.10f, 0.13f));
        drawText(context, 44, 526, "TIMBRE MAIS AGRESSIVO, MOVIMENTO RAPIDO E RESSONANCIA.", 1.18, Color(0.78f, 0.79f, 0.83f));
    }

    void drawFxPage(const GraphicsContext& context) const
    {
        for (const Control& control : kFxControls) drawControl(context, control);

        drawRect(context, {24, 360, 1052, 202}, Color(0.026f, 0.027f, 0.035f));
        drawBorder(context, {24, 360, 1052, 202}, Color(0.24f, 0.027f, 0.040f), 1.0);
        drawText(context, 44, 388, "SINCRONISMO COM O PROJETO", 1.70, Color(1.0f, 0.10f, 0.13f));
        drawText(context, 44, 420, "A PREVIA E OS EFEITOS USAM O BPM ATUAL INFORMADO PELO FL STUDIO.", 1.22, Color(0.80f, 0.81f, 0.85f));
        drawText(context, 44, 452, "O ARQUIVO MIDI E MUSICAL: DEPOIS DE IMPORTADO, SEGUE QUALQUER BPM.", 1.22, Color(0.80f, 0.81f, 0.85f));
        drawText(context, 44, 500, "FLUXO", 1.52, Color(1.0f, 0.10f, 0.13f));
        drawText(context, 44, 530, "GERAR  -  TOCAR PREVIA  -  EXPORTAR MIDI  -  ARRASTAR AO PIANO ROLL.", 1.18,
                 Color(0.80f, 0.81f, 0.85f));
    }

    void drawSequence(const GraphicsContext& context) const
    {
        char title[160];
        std::snprintf(title, sizeof(title), "%s  %s  RAIZ %s%d  %u PASSOS  GERACAO %u",
                      styleName(static_cast<int>(std::lround(fParameters[pStyle]))),
                      scaleName(static_cast<int>(std::lround(fParameters[pScale]))),
                      noteName(static_cast<int>(std::lround(fParameters[pRoot]))),
                      static_cast<int>(std::lround(fParameters[pRoot])) / 12 - 1,
                      fPattern.length, fPattern.generation);
        drawText(context, 30, 214, title, 1.22, Color(0.86f, 0.87f, 0.90f));

        const UiRect area {24, 238, 1052, 198};
        drawRect(context, area, Color(0.008f, 0.009f, 0.013f));
        drawBorder(context, area, Color(0.32f, 0.025f, 0.042f), 1.0);
        for (int i = 1; i < 8; ++i)
            drawRect(context, {area.x, area.y + i * area.h / 8.0, area.w, 1}, Color(0.10f, 0.024f, 0.031f));

        const uint32_t length = fPattern.length;
        const double stepWidth = area.w / static_cast<double>(length);
        for (uint32_t i = 0; i < length; ++i)
        {
            const MelodyStep& step = fPattern.steps[i];
            const double x = area.x + i * stepWidth;
            drawRect(context, {x, area.y, 1.0, area.h},
                     i % 4u == 0u ? Color(0.42f, 0.028f, 0.046f) : Color(0.11f, 0.022f, 0.030f));
            if (step.active == 0u) continue;

            const double noteNorm = clampf((static_cast<float>(step.note) - 24.0f) / 72.0f, 0.0f, 1.0f);
            const double y = area.y + area.h - 15.0 - noteNorm * (area.h - 30.0);
            const double velocity = static_cast<double>(step.velocity) / 127.0;
            drawRect(context, {x + 2.0, y, std::max(3.0, stepWidth - 4.0), 11.0},
                     Color(0.58f + static_cast<float>(velocity) * 0.34f, 0.014f, 0.032f));
        }
    }

    void drawStatusAndMaster(const GraphicsContext& context) const
    {
        drawRect(context, {18, 616, 1064, 52}, Color(0.022f, 0.023f, 0.030f));
        drawBorder(context, {18, 616, 1064, 52}, Color(0.32f, 0.030f, 0.045f), 1.0);
        drawText(context, 30, 628, fStatus, 1.08, Color(0.86f, 0.87f, 0.90f));
        if (!fLastMidiPath.empty())
            drawText(context, 30, 650, "ARQUIVO MIDI NA PASTA DOCUMENTOS / MAKER TRANCE", 0.98, Color(0.67f, 0.69f, 0.73f));

        const float normalized = valueToNormalized(pMaster, fParameters[pMaster]);
        drawRect(context, kMasterControl, Color(0.031f, 0.032f, 0.041f));
        drawBorder(context, kMasterControl, Color(0.25f, 0.25f, 0.29f), 1.0);
        drawText(context, 850, 631, "MASTER", 1.08, Color(0.82f, 0.83f, 0.87f));
        const UiRect track {920, 638, 142, 10};
        drawRect(context, track, Color(0.008f, 0.009f, 0.012f));
        drawRect(context, {track.x, track.y, track.w * normalized, track.h}, Color(0.80f, 0.018f, 0.034f));
        drawRect(context, {track.x + track.w * normalized - 1.0, track.y - 2, 3, track.h + 4}, Color(1.0f, 0.16f, 0.19f));
    }

    std::array<float, kParameterCount> fParameters {};
    MelodyPattern fPattern {};
    int fActiveParameter = -1;
    int fPage = 0;
    double fScale = 1.0;
    double fOffsetX = 0.0;
    double fOffsetY = 0.0;
    char fStatus[256] {};
    std::string fLastMidiPath;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MakerTranceUI)
};

UI* createUI()
{
    return new MakerTranceUI();
}

END_NAMESPACE_DISTRHO
