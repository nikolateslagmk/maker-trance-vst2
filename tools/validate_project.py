from __future__ import annotations

import pathlib
import re
import subprocess
import sys
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
]
missing = [str(path.relative_to(ROOT)) for path in required if not path.exists()]
if missing:
    raise SystemExit("Arquivos ausentes: " + ", ".join(missing))

shared = (ROOT / "Source" / "MakerTranceShared.hpp").read_text(encoding="utf-8")
plugin = (ROOT / "Source" / "MakerTrancePlugin.cpp").read_text(encoding="utf-8")
ui = (ROOT / "Source" / "MakerTranceUI.cpp").read_text(encoding="utf-8")
cmake = (ROOT / "CMakeLists.txt").read_text(encoding="utf-8")
workflow = (ROOT / ".github" / "workflows" / "build-vst2.yml").read_text(encoding="utf-8")

checks = {
    "nome do plugin": "Maker Trance by Alza Produz" in (ROOT / "Source" / "DistrhoPluginInfo.h").read_text(encoding="utf-8"),
    "target VST2": "TARGETS vst2" in cmake,
    "target workflow": "MakerTrance-vst2" in workflow,
    "interface": "MakerTranceUI.cpp" in cmake and "DISTRHO_PLUGIN_HAS_UI 1" in (ROOT / "Source" / "DistrhoPluginInfo.h").read_text(encoding="utf-8"),
    "gerador": "generateMelody" in shared and "GENERATE NEW MELODY" in ui,
    "modo automático": "processSequencer" in plugin and "pMode" in shared,
    "três estilos": all(name in shared for name in ("UPLIFTING", "PSYCHEDELIC", "PROGRESSIVE")),
    "runtime estático": "CMAKE_MSVC_RUNTIME_LIBRARY" in cmake,
    "workflow visível": (ROOT / "WORKFLOW-build-vst2.yml").read_text(encoding="utf-8") == workflow,
}
failed = [name for name, ok in checks.items() if not ok]
if failed:
    raise SystemExit("Falharam: " + ", ".join(failed))

# Count actual enum entries before kParameterCount.
enum_match = re.search(r"enum ParameterId[^\{]*\{(.*?)kParameterCount", shared, re.S)
if not enum_match:
    raise SystemExit("Enum de parâmetros não encontrado")
entries = [e.strip() for e in enum_match.group(1).split(",") if e.strip()]
parameter_count = len(entries)
spec_count = shared.count('{"')
if parameter_count < 40 or spec_count < parameter_count:
    raise SystemExit(f"Parâmetros inconsistentes: enum={parameter_count}, specs={spec_count}")

with tempfile.TemporaryDirectory() as tmp:
    exe = pathlib.Path(tmp) / "test_melody"
    cmd = [
        "g++", "-std=c++17", "-O2", "-Wall", "-Wextra", "-Wpedantic",
        str(ROOT / "tools" / "test_melody.cpp"), "-o", str(exe),
    ]
    subprocess.run(cmd, check=True)
    result = subprocess.run([str(exe)], check=True, text=True, capture_output=True)
    print(result.stdout.strip())

print(f"OK: {parameter_count} parâmetros, interface customizada, workflow e fontes consistentes.")
