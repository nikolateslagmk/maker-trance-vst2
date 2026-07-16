# Instalação no FL Studio

1. Extraia o artefato baixado do GitHub Actions.
2. Crie a pasta:

```text
C:\VSTPlugins\Maker Trance\
```

3. Copie para ela:

```text
Maker Trance by Alza Produz.dll
```

4. Abra o PowerShell e desbloqueie o arquivo, caso o Windows o marque como baixado da internet:

```powershell
Unblock-File "C:\VSTPlugins\Maker Trance\Maker Trance by Alza Produz.dll"
```

5. No FL Studio, abra `Options > Manage plugins`.
6. Adicione `C:\VSTPlugins` em `Plugin search paths`.
7. Marque `Verify plugins`, `Rescan previously verified plugins` e `Rescan plugins with errors`.
8. Clique em `Find installed plugins`.
9. Pesquise por `Maker Trance` em `Installed > Generators`.

## Uso

- **MANUAL:** toque por teclado MIDI ou escreva no Piano Roll.
- **AUTO:** pressione Play no FL Studio para o sequenciador começar.
- **GENERATE NEW MELODY:** avança o contador e cria uma nova sequência.
- **MIDI OUT:** encaminha notas geradas; deixe desligado quando quiser somente ouvir o instrumento.
