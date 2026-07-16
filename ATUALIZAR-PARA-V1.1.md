# Atualizar o repositório para a v1.1

Substitua no GitHub os arquivos abaixo pelos arquivos desta pasta:

```text
CMakeLists.txt
Source/DistrhoPluginInfo.h
Source/MakerTranceShared.hpp
Source/MakerTrancePlugin.cpp
Source/MakerTranceUI.cpp
tools/test_melody.cpp
tools/validate_project.py
.github/workflows/build-vst2.yml
WORKFLOW-build-vst2.yml
```

A forma mais segura é apagar os arquivos antigos no repositório e enviar novamente o conteúdo completo desta pasta.

Se a pasta `.github` não for enviada, abra no GitHub:

```text
.github/workflows/build-vst2.yml
```

Edite o arquivo e copie o conteúdo de `WORKFLOW-build-vst2.yml`.

Depois do commit, aguarde a execução verde em:

```text
Actions > Build Maker Trance VST2 Windows
```

Baixe o artefato da execução nova e substitua a DLL antiga no computador.
