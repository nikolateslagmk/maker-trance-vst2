# Atualizar o repositório para v1.2

Substitua no GitHub estes arquivos:

```text
CMakeLists.txt
README.md
CHANGELOG.md
Source/DistrhoPluginInfo.h
Source/MakerTranceShared.hpp
Source/MakerTranceMidi.hpp
Source/MakerTrancePlugin.cpp
Source/MakerTranceUI.cpp
tools/test_melody.cpp
tools/validate_project.py
```

Mantenha o workflow atual da compilação DPF para Windows x64.

Depois do commit, abra **Actions**, aguarde a execução mais recente ficar verde e baixe o novo artefato. Substitua a DLL antiga no computador antes de abrir o FL Studio.
