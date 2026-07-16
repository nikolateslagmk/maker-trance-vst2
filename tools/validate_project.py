from __future__ import annotations

import pathlib
import re
import subprocess
import tempfile

ROOT = pathlib.Path(__file__).resolve().parents[1]
required = [
    ROOT / "CMakeLists.txt",
    ROOT / "Source" / "DistrhoPluginInfo.h",
    ROOT / "Source" / "MakerTranceShared.hpp",
    ROOT / "Source" / "MakerTranceMidi.hpp",
    ROOT / "Source" / "MakerTrancePlugin.cpp",
    ROOT / "Source" / "MakerTranceUI.cpp",
    ROOT / ".github" / "workflows" / "build-vst2.yml",
    ROOT / "WORKFLOW-build-vst2.yml",
    ROOT / "tools" / "test_melody.cpp",
]
missing = [str(path.relative_to(ROOT)) for path in required if not path.exists()]
if missing:
    raise SystemExit("Arquivos ausentes: " + ", ".join(missing))

shared = (ROOT / "Source" / "MakerTranceShared.hpp").read_text(encoding="utf-8")
midi = (ROOT / "Source" / "MakerTranceMidi.hpp").read_text(encoding="utf-8")
plugin = (ROOT / "Source" / "MakerTrancePlugin.cpp").read_text(encoding="utf-8")
ui = (ROOT / "Source" / "MakerTranceUI.cpp").read_text(encoding="utf-8")
info = (ROOT / "Source" / "DistrhoPluginInfo.h").read_text(encoding="utf-8")
cmake = (ROOT / "CMakeLists.txt").read_text(encoding="utf-8")
workflow = (ROOT / ".github" / "workflows" / "build-vst2.yml").read_text(encoding="utf-8")
visible_workflow = (ROOT / "WORKFLOW-build-vst2.yml").read_text(encoding="utf-8")

preview_match = re.search(
    r"void processPreviewSequencer\(.*?\n    \}\n\n    void releasePreviewNote",
    plugin,
    re.S,
)
if not preview_match:
    raise SystemExit("Processador de previa nao encontrado")
preview_body = preview_match.group(0)

checks = {
    "nome do plugin": "Maker Trance by Alza Produz" in info,
    "target VST2": re.search(r"TARGETS\s+vst2", cmake) is not None,
    "acao DPF win64": "distrho/dpf-cmake-action@v1" in workflow and "target: win64" in workflow,
    "interface responsiva": all(token in info for token in (
        "DISTRHO_UI_USER_RESIZABLE 1",
        "DISTRHO_UI_DEFAULT_WIDTH 970",
        "DISTRHO_UI_DEFAULT_HEIGHT 600",
    )) and "setGeometryConstraints(760u, 470u, true)" in ui,
    "botoes de tamanho": all(token in ui for token in ("kSizeSmall", "kSizeMedium", "kSizeLarge")),
    "abas compactas": all(token in ui for token in ("MELODIA", "SINTETIZADOR", "EFEITOS")),
    "botoes funcionais": all(token in ui for token in ("GERAR MELODIA", "TOCAR PREVIA", "PARAR", "EXPORTAR MIDI")),
    "previa interna one-shot": "noteOn(fPreviewNote" in preview_body and "fPreviewActive = false" in preview_body,
    "previa segue BPM": "stepDurationSamples" in preview_body and "beatsPerMinute" in plugin,
    "sem MIDI auto infinito": "writeMidiEvent" not in plugin and "DISTRHO_PLUGIN_WANT_MIDI_OUTPUT 0" in info,
    "exportacao MIDI": "writeMidiFile" in midi and "{'M','T','h','d'}" in midi and "{'M','T','r','k'}" in midi,
    "piano roll musical": "stepDurationTicks" in shared and "ARRASTE O ARQUIVO" in ui,
    "tres estilos": all(name in shared for name in ("UPLIFTING", "PSYCHEDELIC", "PROGRESSIVE")),
    "shell windows": 'target_link_libraries("MakerTrance-ui" PRIVATE shell32)' in cmake,
    "workflow visivel": visible_workflow == workflow,
}
failed = [name for name, ok in checks.items() if not ok]
if failed:
    raise SystemExit("Falharam: " + ", ".join(failed))

enum_match = re.search(r"enum ParameterId[^\{]*\{(.*?)kParameterCount", shared, re.S)
if not enum_match:
    raise SystemExit("Enum de parametros nao encontrado")
entries = [e.strip() for e in enum_match.group(1).split(",") if e.strip()]
parameter_count = len(entries)
spec_count = shared.count('{"')
if parameter_count < 44 or spec_count < parameter_count:
    raise SystemExit(f"Parametros inconsistentes: enum={parameter_count}, specs={spec_count}")

with tempfile.TemporaryDirectory() as tmp:
    exe = pathlib.Path(tmp) / "test_melody"
    cmd = [
        "g++", "-std=c++17", "-O2", "-Wall", "-Wextra", "-Wpedantic",
        str(ROOT / "tools" / "test_melody.cpp"), "-o", str(exe),
    ]
    subprocess.run(cmd, check=True)
    result = subprocess.run([str(exe)], check=True, text=True, capture_output=True, cwd=tmp)
    print(result.stdout.strip())

print(
    f"OK: {parameter_count} parametros; previa one-shot; exportacao MIDI; "
    "BPM sync; interface responsiva; workflow e fontes consistentes."
)
