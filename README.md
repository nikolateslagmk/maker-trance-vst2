# Maker Trance by Alza Produz — VST2 x64

Sintetizador VST2 para FL Studio voltado a **Uplifting Trance**, **Psychedelic Trance** e **Progressive Trance**, com modo manual e gerador automático de melodias sincronizado ao transporte do host.

## Principais recursos

- VST2 Windows x64 em arquivo único `.dll`.
- Interface própria em vermelho e preto, sem fontes ou imagens externas.
- 24 vozes de polifonia.
- Supersaw de até 9 vozes por nota, detune e largura estéreo.
- Segundo oscilador, sub, ruído, drive e filtro ressonante.
- Perfis sonoros específicos para Uplifting, Psychedelic e Progressive.
- Modo **Manual** para tocar ou escrever MIDI normalmente no Piano Roll.
- Modo **Auto** com sequenciador sincronizado ao Play do FL Studio.
- Gerador de melodias com raiz, escala, densidade, movimento, oitavas, gate, swing e humanização.
- Contador de geração de 32 bits: **4.294.967.296 sequências de eventos identificáveis antes de o contador reiniciar**.
- A geração codifica o número da geração em pequenas variações de velocity e gate, impedindo a repetição exata do mesmo padrão de eventos durante esse ciclo.
- MIDI Out opcional para encaminhar as notas geradas ao host quando suportado.
- Trance Gate, chorus, delay ping-pong e reverb.
- Seis programas de fábrica.
- Runtime MSVC estático para reduzir dependências externas.

> Nenhum sistema finito pode prometer novidade infinita. Este projeto oferece um ciclo prático de mais de 4,29 bilhões de gerações distintas antes de qualquer reinício do contador.

## Programas de fábrica

1. Uplifting Spirit
2. Uplifting Pluck
3. Psy Cosmic Acid
4. Psy Astral Sequence
5. Progressive Horizon
6. Progressive Deep Flow

## Comportamento do modo automático

1. Selecione **AUTO** na interface.
2. Escolha UPLIFT, PSY ou PROG.
3. Defina raiz, escala, rate, comprimento e demais parâmetros.
4. Clique em **GENERATE NEW MELODY**.
5. Pressione Play no FL Studio.

O gerador toca internamente o sintetizador. Ative **MIDI OUT** somente quando desejar rotear ou gravar as notas geradas no FL Studio.

## Compilação recomendada

Use o GitHub Actions incluído em:

```text
.github/workflows/build-vst2.yml
```

Como a pasta `.github` pode ficar oculta no Windows, uma cópia visível do mesmo workflow está na raiz:

```text
WORKFLOW-build-vst2.yml
```

Consulte `CRIAR-WORKFLOW-NO-GITHUB.md` para o procedimento exato.

## Instalação no FL Studio

Copie a DLL gerada para, por exemplo:

```text
C:\VSTPlugins\Maker Trance\Maker Trance by Alza Produz.dll
```

No FL Studio:

1. Abra `Options > Manage plugins`.
2. Adicione `C:\VSTPlugins` aos caminhos de busca.
3. Ative a verificação de plugins e clique em `Find installed plugins`.
4. Pesquise por `Maker Trance`.

Este plugin é x64 e requer o FL Studio de 64 bits.

## Estrutura

```text
Source/MakerTrancePlugin.cpp   DSP, sintetizador, sequenciador e efeitos
Source/MakerTranceUI.cpp       interface vermelha/preta
Source/MakerTranceShared.hpp   parâmetros e gerador de melodias compartilhado
Source/DistrhoPluginInfo.h     metadados do plugin
CMakeLists.txt                 build DPF VST2
.github/workflows/...          compilação automática Windows
```

## Estabilidade

O processamento de áudio não cria memória durante o callback. As linhas de delay e reverb são reservadas na ativação, o gerador usa estruturas fixas e o projeto limita a polifonia a 24 vozes para equilibrar riqueza sonora e desempenho.
