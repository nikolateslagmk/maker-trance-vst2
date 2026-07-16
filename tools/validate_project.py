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
plugin = (ROOT / "Source" / "MakerTrancePlugin.cpp").read_text(encoding="utf-8")
ui = (ROOT / "Source" / "MakerTranceUI.cpp").read_text(encoding="utf-8")
info = (ROOT / "Source" / "DistrhoPluginInfo.h").read_text(encoding="utf-8")
cmake = (ROOT / "CMakeLists.txt").read_text(encoding="utf-8")
workflow = (ROOT / ".github" / "workflows" / "build-vst2.yml").read_text(encoding="utf-8")
visible_workflow = (ROOT / "WORKFLOW-build-vst2.yml").read_text(encoding="utf-8")

process_match = re.search(
    r"void processSequencer\(.*?\n    \}\n\n    void sendMidi",
    plugin,
    re.S,
)
if not process_match:
    raise SystemExit("Processador MIDI one-shot nao encontrado")
process_body = process_match.group(0)

checks = {
    "nome do plugin": "Maker Trance by Alza Produz" in info,
    "target VST2": re.search(r"TARGETS\s+vst2", cmake) is not None,
    "acao DPF win64": "distrho/dpf-cmake-action@v1" in workflow and "target: win64" in workflow,
    "interface 1280x760": "DISTRHO_UI_DEFAULT_WIDTH 1280" in info and "DISTRHO_UI_DEFAULT_HEIGHT 760" in info,
    "gerador": "generateMelody" in shared and "GENERATE NEW MELODY" in ui,
    "modo MIDI one-shot": "fSequenceFinished" in process_body and "% fPattern.length" not in process_body,
    "auto nao dispara sintetizador": "noteOn(" not in process_body,
    "sincronismo BPM": "stepDurationSamples" in process_body and "beatsPerMinute" in plugin,
    "modo manual padrao": '{"Play Mode", "mode", "0=Manual synth 1=One-shot MIDI capture", 0.0f, 1.0f, 0.0f' in shared,
    "instrucoes piano roll": "BURN MIDI TO NEW PATTERN" in ui,
    "tres estilos": all(name in shared for name in ("UPLIFTING", "PSYCHEDELIC", "PROGRESSIVE")),
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
if parameter_count < 40 or spec_count < parameter_count:
    raise SystemExit(f"Parametros inconsistentes: enum={parameter_count}, specs={spec_count}")

with tempfile.TemporaryDirectory() as tmp:
    exe = pathlib.Path(tmp) / "test_melody"
    cmd = [
        "g++", "-std=c++17", "-O2", "-Wall", "-Wextra", "-Wpedantic",
        str(ROOT / "tools" / "test_melody.cpp"), "-o", str(exe),
    ]
    subprocess.run(cmd, check=True)
    result = subprocess.run([str(exe)], check=True, text=True, capture_output=True)
    print(result.stdout.strip())

print(
    f"OK: {parameter_count} parametros; MIDI one-shot; BPM sync; "
    "interface ampliada; workflow e fontes consistentes."
)
