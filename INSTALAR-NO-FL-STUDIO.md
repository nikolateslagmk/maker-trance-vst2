# Instalação e captura da melodia no FL Studio

## Instalar a DLL

1. Feche o FL Studio.
2. Copie `Maker Trance by Alza Produz.dll` para:

```text
C:\VSTPlugins\Maker Trance\
```

3. Abra o FL Studio.
4. Vá a `Options > Manage plugins`.
5. Adicione `C:\VSTPlugins` ao Plugin search path.
6. Clique em `Find installed plugins`.

## Gerar notas no Piano roll

1. Abra o plugin no Channel Rack.
2. No Wrapper, configure um MIDI Output Port, por exemplo `1`.
3. No plugin, escolha estilo, raiz, escala, rate e comprimento.
4. Clique em `GENERATE NEW MELODY`.
5. Selecione `AUTO MIDI`.
6. Clique com o botão direito no canal do Maker Trance.
7. Escolha `Burn MIDI to > New pattern`.
8. Quando as notas forem criadas, volte ao plugin e selecione `MANUAL SYNTH`.

A melodia passa a existir no Piano roll e só toca pelas notas gravadas no padrão.

## BPM

O AUTO MIDI calcula os eventos com o BPM recebido do FL Studio. Depois que as notas estão no Piano roll, elas ficam presas à grade musical e acompanham qualquer mudança de BPM do projeto.
