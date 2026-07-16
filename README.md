# Maker Trance by Alza Produz — VST2 x64 v1.2

Sintetizador para **Uplifting Trance**, **Psychedelic Trance** e **Progressive Trance**, com gerador de melodias, prévia sincronizada ao BPM do FL Studio e exportação em arquivo MIDI para o Piano Roll.

## Correções da v1.2

- Interface responsiva e redimensionável.
- Tamanho inicial reduzido para `970 × 600`.
- Botões **S**, **M** e **L** para mudar o tamanho mesmo quando o host não oferece uma alça de redimensionamento.
- Interface organizada em três abas: **Melodia**, **Sintetizador** e **Efeitos**.
- Botões funcionais **Gerar melodia**, **Tocar prévia**, **Parar** e **Exportar MIDI**.
- A prévia toca uma única vez e não depende do Play do projeto.
- A prévia usa o BPM atual informado pelo FL Studio e acompanha mudanças de BPM entre os passos.
- O modo automático antigo e a saída MIDI em loop foram removidos.
- O botão **Exportar MIDI** gera um arquivo `.mid` e abre o Explorer com o arquivo selecionado.
- O arquivo MIDI usa posições musicais em PPQ; depois de colocado no Piano Roll, acompanha qualquer BPM do projeto.

## Como usar

1. Abra a aba **MELODIA**.
2. Escolha Uplift, Psy ou Prog.
3. Ajuste raiz, escala, divisão, passos, densidade e demais controles.
4. Clique em **GERAR MELODIA**.
5. Clique em **TOCAR PREVIA** para ouvir uma vez usando o BPM atual do FL Studio.
6. Clique em **PARAR** para interromper imediatamente.
7. Clique em **EXPORTAR MIDI**.
8. O Windows abre a pasta `Documentos\Maker Trance` com o arquivo selecionado.
9. Arraste o arquivo `.mid` para o Piano Roll ou para o Channel Rack do FL Studio.

## Sobre o botão de importação

O padrão VST2 não oferece uma função universal para um plugin escrever diretamente dentro do Piano Roll do host. Por isso, a solução confiável é criar um arquivo MIDI e abrir a pasta automaticamente. O produtor só precisa arrastar o arquivo selecionado para o Piano Roll.

## Compilação

O workflow está em:

```text
.github/workflows/build-vst2.yml
```

A cópia visível permanece em:

```text
WORKFLOW-build-vst2.yml
```

O GitHub Actions gera o arquivo:

```text
Maker Trance by Alza Produz.dll
```

## Instalação

Copie a DLL para:

```text
C:\VSTPlugins\Maker Trance\Maker Trance by Alza Produz.dll
```

No FL Studio, adicione `C:\VSTPlugins` aos caminhos do Plugin Manager e faça uma nova varredura.
