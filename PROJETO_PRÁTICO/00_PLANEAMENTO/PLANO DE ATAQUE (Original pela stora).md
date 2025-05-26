Trabalho para codificar com arrays (**Aparentemente não com listas ligadas????**)

O programa pretende gestão de livros de uma biblioteca escolar, nomeadamente:
   - na requisição de livros
   - na devolução de livros.

O sistema é utilizado por dois tipos de perfis:
   Aluno:
      - ID único
      - Nome
      - Palavra-passe
   Bibliotecário

Livros:
   - ID único
   - Nome
   - Descrição
   - Estado (disponível ou indisponível)
   - ID do Aluno que requisitou (se estiverem requisitados)

Caso o livro esteja indisponível (requisitado), é mantida uma fila de espera com
os IDs dos alunos que solicitaram esse livro.

O sistema deve permitir que os alunos possam consultar a lista de livros existentes na
biblioteca, sendo possível:
   - Listar todos os livros por ordem alfabética do nome;
   - Listar apenas os livros disponíveis;
   - Consultar as informações de um livro (através do uID), incluindo o número de alunos que se encontram na fila de espera.

Um aluno pode:
   - Requisitar um livro, desde que este se encontre disponível (**E então para que serve a lista de espera**)
   - Quando requisitado:
      - O estado do livro passa a indisponível
      - uID do aluno é registado como requisitante.

Caso o livro esteja indisponível, o ID do aluno é colocado numa fila de espera associada ao livro.

Sempre que um livro é requisitado, o sistema deve registar a operação num ficheiro de texto:
   - onde cada linha contém o nome do livro, o ID do aluno e a data da requisição.
      - Tipo `[+] 2025-05-25 REQ uID_Aluno NomeLivro`?


A devolução de livros é feita exclusivamente pelo bibliotecário (**A requisição tmb deveria ser, então!!**).
Quando um livro é devolvido:
   - Se não existir fila de espera:
      - O livro volta ao estado de disponível
   - Se existir fila de espera:
      - o livro passa a estar requisitado ao primeiro aluno da fila, atualizando o ID do aluno associado ao livro.
   - O sistema deve registar a operação num ficheiro de texto onde cada linha contém o nome do livro, o ID do aluno e a data da requisição.
      - Tipo `[-] 2025-05-25 DEV uID_Aluno NomeLivro`


O bibliotecário deve também poder:
   - Registar livros
   - Remover um livro dado o seu ID
   - Consultar a lista completa de livros ordenados por ID
   - Consultar histórico de devoluções
   - Remover do histórico as últimas devoluções efetuadas, com base num número fornecido pelo bibliotecário. (**Deveria ser das mais antigas, não das mais recentes, diria eu**)


A aplicação deve ainda garantir que o seu estado de execução é guardado em ficheiro para que mais tarde possa ser restaurado, garantindo assim continuidade de funcionamento entre utilizações.

Elementos de Avaliação
Os grupos deverão submeter os seguintes elementos até ao prazo de entrega:
   - um relatório final (máximo de duas páginas A4) no qual devem fazer uma reflexão  sobre a forma como o desenvolvimento do trabalho decorreu, bem como uma autoavaliação, devidamente fundamentada;
   - o código fonte C, devidamente comentado;
   - o executável da aplicação (**Pois, mas vai compilado para Linux ou MacOS. A stora não iria conseguir correr...**). 
   - Todos os ficheiros produzidos e sujeitos a avaliação devem ser submetidos no moodle.

 
Fatores de valorização:
   - a estruturação do código fonte;
   - a qualidade e correção da programação em C;
   - o cumprimento dos requisitos deste enunciado;
   - a interface com o utilizador (facilidade de utilização da aplicação);


Grupos de Trabalho:
   - Deverá ser desenvolvido em grupo, constituído por max. três alunos.
   - A constituição dos grupos deverá ser entregue ao docente da disciplina na aula ou por email.

Datas Importantes
   - 15 de junho de 2025: data limite para a submissão final do trabalho prático no moodle.