# Quando a pasta .github não for enviada

A pasta `.github` é oculta no Windows e alguns navegadores não a enviam. Não envie o projeto zipado esperando que o GitHub extraia.

## Método seguro

1. Envie para o repositório as pastas `Source` e `tools`, além dos arquivos da raiz.
2. No GitHub, clique em `Add file > Create new file`.
3. No nome do arquivo, digite exatamente:

```text
.github/workflows/build-vst2.yml
```

4. Abra o arquivo visível `WORKFLOW-build-vst2.yml` deste pacote.
5. Copie todo o conteúdo e cole no editor do GitHub.
6. Clique em `Commit changes`.
7. Abra `Actions > Build Maker Trance VST2 Windows`.
8. Clique em `Run workflow > Run workflow`.
9. Quando ficar verde, abra a execução e baixe o artefato:

```text
Maker-Trance-by-Alza-Produz-Windows-VST2
```

O artefato conterá:

```text
Maker Trance by Alza Produz.dll
```
