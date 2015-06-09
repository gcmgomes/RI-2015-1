# RI-2015-1
//--------------------------
Restrições
--------------------------//

A única restrição (temporária), é que o caminho para o diretório que contem esse
trabalho não possua nenhum espaço em branco.

//--------------------------
Geração de binários
--------------------------//

Para poder gerar os binários necessários para a execução do trabalho, é
necessário que a biblioteca Gumbo, disponível em

https://github.com/google/gumbo-parser

esteja previamente instalada no diretório padrão do compilador.

Após instalada, basta utilizar o comando

make

e todos os arquivos necessários serão automaticamente gerados.

Caso a compilação falhe com o erro sobre o linking da biblioteca iconv,
basta remover a flag -liconv, que o mesmo funcionará.

//--------------------------
Execução
--------------------------//

Os binários necessários estão localizados nos diversos diretórios */bin, sendo que
cada um deles, quando executado sem parâmetros, imprime as informações de uso do mesmo.
Para realizar a execução de pipelines, basta olhar as primeiras linhas dos arquivos *.bash.
Esses são responsáveis por executar todas as tarefas complexas, como indexação, parsing,
preprocessamentos, execução de consultas e inicialização da interface web de consultas.
