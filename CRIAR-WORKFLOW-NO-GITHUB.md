# Criar ou atualizar o workflow no GitHub

Se a pasta `.github` não for enviada pelo navegador:

1. Abra o repositório na aba `Code`.
2. Entre em `.github/workflows/build-vst2.yml` ou crie esse caminho com `Add file > Create new file`.
3. Copie todo o conteúdo do arquivo visível `WORKFLOW-build-vst2.yml` deste pacote.
4. Cole no editor do GitHub.
5. Clique em `Commit changes`.
6. Abra `Actions > Build Maker Trance VST2 Windows`.
7. Aguarde a execução ficar verde.
8. Baixe o artefato da execução mais recente.

O workflow usa Ubuntu 22.04 e o `dpf-cmake-action` para gerar uma DLL Windows x64.
