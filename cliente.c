#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define TAM_BUFFER 1024

// Cria o socket e estabelece conexão com servidor, checando possíveis erros
int main()
{

	// Variáveis
	int socketCliente;
	struct sockaddr_in endServidor;
	char buffer[TAM_BUFFER];
	int flag0=1, flag1=1;

	// Cria o socket do Cliente
	//		- AF_INET = endereços IPv4
	//		- SOCK_STREAM = protocolo baseado em conexão (TCP)
	socketCliente = socket(AF_INET, SOCK_STREAM, 0);

	// Checa se o socket do Cliente foi criado, alerta caso tenha falhado
	if(socketCliente<0)
	{
		perror(" >> ERRO AO CRIAR O SOCKET << \n");
		exit(1);
	}

	// Armazena o endereço completo do servidor na rede
	//		- AF_INET = endereços IPv4
	//		- PORT = 8080
	//		- Endereço 127.0.0.1 = servidor e cliente rodam na mesma máquina; caso deseje rodar em máquinas diferentes conectadas à mesma rede, mude o endereço IP
	endServidor.sin_family = AF_INET;
	endServidor.sin_port = htons(PORT);
	endServidor.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	// Conecta o cliente ao servidor e verifica se houve erro na conexão
	if(connect(socketCliente, (struct sockaddr*)&endServidor, sizeof(endServidor))<0)
	{
		perror(" >> ERRO AO CONECTAR << \n");
		exit(1);
	}

	// Caso a conexão tenha sido bem sucedida, inicia o terminal 
	printf(" >> CONECTADO AO SERVIDOR \n -- Digite comandos -- \n -- Para sair, digite 'sair' \n");

	// Loop que realiza a troca de dados entre servidor e cliente
	while(flag0)
	{
		printf("[Voce] ");

		// Lê o input do cliente e formata-o
		fgets(buffer, TAM_BUFFER, stdin);					
		buffer[strcspn(buffer, "\n")]='\0';
		
		// Envia o input do cliente e verifica se é o comando para sair
		send(socketCliente, buffer, strlen(buffer), 0);	
		if(strcmp(buffer, "sair")==0)						
			flag0=0;

		// Loop que envia a resposta do servidor ao cliente
		flag1=1;
		printf("[Servidor] \n");
		while(flag1)
		{
			// Lê o socket do servidor e coloca os dados na variável "buffer"
			int bytesRecebidos = recv(socketCliente, buffer, TAM_BUFFER-1, 0);	 
			
			// Verifica se o servidor não encerrou a conexão ou se houve falha na comunicação
			if(bytesRecebidos<=0)						
				flag1=0;

			// Termina de formatar o "buffer"
			buffer[bytesRecebidos]='\0';
			
			// Procura pelo marcador de fim do comando, conforme definido em "servidor.c"
			if(strstr(buffer, "<END>") != NULL) 
				flag1=0;	

			// Mostra o output
			printf("%s", buffer);
		}
		
		printf("\n");

	}
	
	// Fecha o socket do cliente após o fim da conexão
	close(socketCliente);
	return 0;
}
