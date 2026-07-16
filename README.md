# Maker Trance by Alza Produz — VST2 x64 v1.3

Sintetizador VST2 para Windows x64 com interface responsiva, prévia sincronizada ao BPM, exportação MIDI e motor melódico triádico voltado a Uplifting, Psychedelic e Progressive Trance.

## Novo motor Epic Triad

A v1.3 substitui a geração aleatória livre por um sistema musical controlado:

- progressões harmônicas próprias para cada escala;
- melodias construídas prioritariamente sobre tônica, terça e quinta;
- motivos de arpejo repetidos com variação;
- desenho de frase com início, crescimento, clímax e resolução;
- cadência final dominante–tônica;
- condução de vozes com saltos limitados;
- respeito ao alcance de oitavas escolhido;
- rejeição de frases ruins por pontuação musical;
- geração consecutiva sem repetição imediata de desenho melódico;
- assinatura única em velocity e gate até o contador de 32 bits completar o ciclo.

A beleza musical é subjetiva, mas o gerador agora impede sequências sem centro tonal, finais sem resolução, saltos excessivos e notas fora da escala.

## Fluxo no FL Studio

1. Clique em **GERAR MELODIA**.
2. Clique em **TOCAR PRÉVIA** para ouvir uma vez no BPM atual do projeto.
3. Clique em **EXPORTAR MIDI**.
4. Arraste o arquivo `.mid` para o Piano Roll.
5. Depois de importada, a melodia acompanha qualquer mudança de BPM do projeto.

## Compilação

O workflow `.github/workflows/build-vst2.yml` usa o DPF oficial para gerar a DLL Windows x64.

O artefato final contém:

```text
Maker Trance by Alza Produz.dll
```

Instale a DLL em uma pasta VST2 incluída no Plugin Manager do FL Studio, por exemplo:

```text
C:\VSTPlugins\Maker Trance\Maker Trance by Alza Produz.dll
```
