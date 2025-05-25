Trabalho para codificar com arrays (**Aparentemente não com listas ligadas????**)

O sistema é utilizado por dois tipos de perfis:
   - Aluno:
      - ID único (**Auto ID ou número escolar??**)
      - Nome
      - Palavra-passe
   - Bibliotecário:
      - ID único (**Tem de ser ter cuidado ao cruzamento de uIDs com alunos. Mas se calhar põe-se tmb numero de escola, acho que stores tmb têm**)
      - Nome (**Será que vale a pena, a stora não mencionou isto**)
      - Palavra-passe

Livros:
   - ID único (**Tipo ISBN ou auto?**)
   - Nome
   - Descrição
   - Estado (disponível ou indisponível)
   - uID do Aluno que requisitou (se estiverem requisitados)
   - Caso o livro esteja indisponível (requisitado), é mantida uma fila de espera com os uIDs dos alunos que solicitaram esse livro.

O programa pretende gestão de livros de uma biblioteca escolar, nomeadamente:
   - na requisição de livros &nbsp; &nbsp; [Aluno]<br>
   - na devolução de livros &nbsp; &nbsp; [Bibleotecário]<br>
   - gestão dos livros no sistema: <br>
      - Adição de livros&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;&nbsp;[Bibleotecário]<br>
      - Remoção de livros (através de uID)&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;[Bibleotecário]<br>
      - Listar livros po uID&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;&nbsp; &nbsp; &nbsp; &nbsp; [Bibleotecário]<br>
      - Listar livros por ordem alfabética&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;&nbsp; &nbsp;[Aluno/Bibleotecário]<br>
      - Listar apenas livros disponíveis (**Qualquer ordem??**)&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;&nbsp; &nbsp; [Aluno/Bibleotecário]<br>
      - Consultar info de um livro (através do uID)&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;[Aluno/Bibleotecário]<br>
        incluindo o número de alunos que se encontram na fila de espera.
      - Consultar histórico de devoluções &nbsp; &nbsp; &nbsp; &nbsp;&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;[Bibleotecário]<br>
      - Remover do histórico as últimas X devoluções efetuadas &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;&nbsp;[Bibleotecário]<br>
        (**Deveria ser das mais antigas, não das mais recentes, diria eu**)


Quando um livro é requisitado:
   - O estado do livro passa a indisponível
   - uID do aluno é registado como requisitante.
   - Caso o livro esteja indisponível, o ID do aluno é colocado numa fila de espera associada ao livro.
   - Sistema deve registar a operação num ficheiro de texto onde cada linha contém o nome do livro, o ID do aluno e a data da requisição.
      - Tipo `[+] 2025-05-25 14:54 REQ uID_Aluno NomeLivro`?
      - Apenas quando requisita (muda de "dono") ou tmb quando adiciona à queue?
        - Tipo `[+] 2025-05-25 14:54 QUE uID_Aluno NomeLivro`?

    
A devolução de livros é feita exclusivamente pelo bibliotecário (**A requisição tmb deveria ser, então!!**).
Quando um livro é devolvido:
   - Se não existir fila de espera:
      - O livro volta ao estado de disponível
   - Se existir fila de espera:
      - o livro passa a estar requisitado ao primeiro aluno da fila, atualizando o ID do aluno associado ao livro.
        - E no registo presumo que se tenha de fazer algo tipo:
          - `[-] 2025-05-25 14:54 ARQ uID_Aluno NomeLivro`? (**Auto ReQuest, ou entao um DQU, de DeQUeue**)
   - O sistema deve registar a operação num ficheiro de texto onde cada linha contém o nome do livro, o ID do aluno e a data da requisição.
      - Tipo `[-] 2025-05-25 14:54 DEV uID_Aluno NomeLivro`


A aplicação deve ainda garantir que o seu estado de execução é guardado em ficheiro para que mais tarde possa ser restaurado, garantindo assim continuidade de funcionamento entre utilizações.
(**Ou então trabalhar mais diretamente com ficheiros, tipo criar um sistema de base de dados**)

Elementos de Avaliação
Os grupos deverão submeter os seguintes elementos até ao prazo de entrega:
   - um relatório final (máximo de duas páginas A4) no qual devem fazer uma reflexão sobre a forma como o desenvolvimento do trabalho decorreu, bem como uma autoavaliação, devidamente fundamentada;
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
   - A constituição dos grupos deverá ser entregue na aula ou por email.

Datas Importantes
   - 15 de junho de 2025: data limite para a submissão final do trabalho prático no moodle. (**Mas no moodle dizia 22...**)
