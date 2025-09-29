#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define TAM_BUFFER 1024

int main()
{
	int socketCliente;
	struct sockaddr_in endServidor;
	char buffer[TAM_BUFFER];

	socketCliente = socket(AF_INET, SOCK_STREAM, 0);

	if(socketCliente<0)
	{
		perror(" >> ERRO AO CRIAR O SOCKET << \n");
		exit(1);
	}

	endServidor.sin_family = AF_INET;
	endServidor.sin_port = htons(PORT);
	endServidor.sin_addr.s_addr = inet_addr("127.0.0.1");

	if(connect(socketCliente, (struct sockaddr*)&endServidor, sizeof(endServidor))<0)
	{
		perror(" >> ERRO AO CONECTAR << \n");
		exit(1);
	}
	printf(" >> CONECTADO AO SERVIDOR \n -- Digite comandos -- \n -- Para sair, digite 'sair' \n");
	while(1)
	{
		printf("[Voce] ");
		fgets(buffer, TAM_BUFFER, stdin);
		buffer[strcspn(buffer, "\n")]='\0';

		send(socketCliente, buffer, strlen(buffer), 0);

		if(strcmp(buffer, "sair")==0)
			break;

		printf("[Servidor] \n");

		while(1)
		{
			int bytesRecebidos = recv(socketCliente, buffer, TAM_BUFFER-1, 0);
			if(bytesRecebidos<=0) 
				break;
			buffer[bytesRecebidos]='\0';

			if(strstr(buffer, "<END-OUTPUT>") != NULL) 
				break;

			printf("%s", buffer);
		}
		
		printf("\n");

	}

	close(socketCliente);
	return 0;
}
