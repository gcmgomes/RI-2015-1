# RI-2015-1
//--------------------------
Geração de binários
//--------------------------

Para poder gerar os binários necessários para a execução do trabalho, é
necessário que a biblioteca Gumbo, disponível em

https://github.com/google/gumbo-parser

esteja previamente instalada no diretório padrão do compilador.

Após instalada, basta utilizar o comando

make

e todos os arquivos necessários serão automaticamente gerados.

//--------------------------
Execução
//--------------------------

Para poder gerar o índice invertido, basta executar o comando

bash index.bash [caminho da coleção] [índice da coleção]

Arquivos de índice, tuplas e vocabulario serão gerados no diretório data/.

Para poder realizar consultas ao índice, basta executar o comando

bash query\_processor.bash

Após carregadas as informações necessárias em memória, ele estará pronto
para consultas. Quando não se deseja mais realizar nenhuma consulta, transmita
o fim de arquivo (EOF) para o programa (Ctrl-D em ambientes linux).

Para os demais binários (disponíveis em \*/bin/), uma execução sem parâmetros
indica quais são os argumentos necessários para seu correto funcionamento.
