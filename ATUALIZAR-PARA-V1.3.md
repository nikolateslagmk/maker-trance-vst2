# Atualizar o repositório para v1.3

Substitua estes arquivos no GitHub:

```text
Source/MakerTranceShared.hpp
Source/MakerTrancePlugin.cpp
Source/MakerTranceUI.cpp
tools/test_melody.cpp
tools/validate_project.py
CMakeLists.txt
README.md
CHANGELOG.md
BUILD-STATUS.md
```

Depois faça o commit. O GitHub Actions iniciará uma nova compilação. Baixe apenas o artefato da execução mais recente e substitua a DLL antiga no diretório VST2.
