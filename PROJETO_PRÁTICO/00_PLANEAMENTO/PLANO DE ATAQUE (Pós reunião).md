#### Aluno
| Campo  | Tipo     | Nota                                                                  |
|--------|----------|-----------------------------------------------------------------------|
| uID    | uint16_t | número escolar (ddddd), e este vai até 63xxx, por isso mais que chega |
| nome   | char[??] | a definir quanto                                                      |
| passwd | char[33] | md5_hash (32 + /0)                                                    |


#### Bibliotecário (**Falta confirmar a estrutura com a professora**)
| Campo  | Tipo     | Nota                                                                  |
|--------|----------|-----------------------------------------------------------------------|
| uID    | uint16_t | número escolar (ddddd), e este vai até 63xxx, por isso mais que chega |
| nome   | char[??] | (**A confirmar pela stora se é preciso**)                             |
| passwd | char[33] | md5_hash (32 + /0)                                                    |


#### Livros
| Campo                  | Tipo             | Nota                                                      |
|------------------------|------------------|-----------------------------------------------------------|
| uID                    | uint64_t         | para acomodar ISBN-13 tal como ISBN-10                    |
| nome                   | char[??]         | a definir quanto                                          |
| uID_Aluno_requisitante | uint16_t or NULL | Se NULL: disponível; else indisponível;                   |
| waiting_queue          | LinkedList       | Lista com os ids dos alunos em espera para requisitar     |
| description            | char[??]         | A definir quanto, ou então ler diretamente de um ficheiro |


#### Funções base do programa
| Função                                                                                                                      | Perfil Autorizados   |
|-----------------------------------------------------------------------------------------------------------------------------|----------------------|
| Requisição de livros                                                                                                        | Aluno                |
| Devolução de livros                                                                                                         | Bibleotecário        |
| Adição de livros                                                                                                            | Bibleotecário        |
| Remoção de livros (através de uID)                                                                                          | Bibleotecário        |
| Listar livros po uID                                                                                                        | Bibleotecário        |
| Listar livros por ordem alfabética                                                                                          | Aluno/Bibleotecário  |
| Listar apenas livros disponíveis (**Qualquer ordem??**)                                                                     | Aluno/Bibleotecário  |
| Consultar info de um livro (através do uID) incluindo o número de alunos que se encontram na fila de espera.                | Aluno/Bibleotecário  |
| Consultar histórico de devoluções                                                                                           | Bibleotecário        |
| Remover do histórico as últimas X devoluções efetuadas (**Deveria ser das mais antigas, não das mais recentes, diria eu**)  | Bibleotecário        |

#### Quando um livro é requisitado:
1. O estado do livro passa a indisponível
2. uID do aluno é registado como requisitante.
3. Caso o livro esteja indisponível, o ID do aluno é colocado numa fila de espera associada ao livro.
4. Sistema deve registar a operação num ficheiro de texto onde cada linha contém: Nome do livro, ID do aluno, data da requisição.
   - Tipo `[*] 2025-05-25 14:54 REQ uID_Aluno NomeLivro`?
   - Apenas quando requisita (muda de "dono") ou tmb quando adiciona à queue?
   - Tipo `[*] 2025-05-25 14:54 QUE uID_Aluno NomeLivro`?


#### Quando um livro é devolvido:
1. o livro passa a estar requisitado ao primeiro aluno da fila, atualizando o ID do aluno associado ao livro.
   Ao estar vazia a lista, o primeiro termo será NULL, portanto resolvido.
2. O sistema regista a operação num ficheiro de texto onde cada linha contém o nome do livro, o ID do aluno e a data da entrega.
   - Tipo `[*] 2025-05-25 14:54 DEV uID_Aluno NomeLivro`
   - E no registo presumo que tmb se tenha de fazer algo tipo:
   - `[*] 2025-05-25 14:54 ARQ uID_Aluno NomeLivro`? (**Auto ReQuest, ou entao um DQU, de DeQUeue**)
      
   
A aplicação deve ainda garantir que o seu estado de execução é guardado num ficheiro para que mais tarde
possa ser restaurado, garantindo assim continuidade de funcionamento entre utilizações.
(**Ou então trabalhar mais diretamente com ficheiros, tipo criar um sistema de base de dados**)
