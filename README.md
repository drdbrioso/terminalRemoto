# terminalRemoto - Trabalho 1 de Redes de Computadores
## Alunos:
- Bruno Calabria Cortez Navas (13783947)
- Eduardo Brioso Luceiro (14607621)
- Heloisa Pazeti (15477991)
- Miguel Rodrigues Fonseca (14682196)

## Objetivo do Projeto

O intuito do projeto é ser um terminal remoto que recebe e executa comandos de diversos clientes. Utilizamos o conceito de socket para simular a conexão entre um cliente e um servidor. 

Foram programados dois lados, o cliente e o servidor. Do lado servidor temos as conexões feitas em múltiplos clientes, que por sua vez podem se conectar a porta do servidor e, após verificada a conexão, podem digitar diversos comandos e receber as respostas para eles em seu terminal.

### Sockets e Threads

Sockets podem ser entendidos como canais de comunicação. São utilizados para enviar informação entre aplicações locais ou através da rede. Cada socket tem seu próprio "nome", para que a aplicação possa encontrá-lo e chamá-lo quando for necessário. 

Em nosso código, no arquivo cliente.c, temos esse socket sendo criado e atribuido seu "nome". Em seguida, conectamos esse cliente ao endereço do servidor. Após a conexão feita basta que os clientes digitem comandos, esses serão passados como threads - pequenas partes do processo como um todo - que serão executados pelo servidor, que enviará pelo socket a resposta à essas requisições.

Isso pode ser feito por múltiplos clientes ao mesmo tempo, assim como nas aplicações gerais reais.

## Detalhes de Utilização

O código foi escrito e compilado na versão _linux 6.16.8.arch3-1_ do kernel Linux e na versão _gcc 15.2.1 20250813_ do compilador.

Para executar, entre no diretório com os arquivos do programa e execute o comando _make_. Em seguida, abra o servidor em um terminal (_./servidor_) e os clientes em outros (_./cliente_).



