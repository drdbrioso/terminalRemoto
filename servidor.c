#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netinet/in.h>

#define PORT 8080 
#define TAM_BUFFER 1024

void *handle_client(void *arg)
{
	int flag=1;
	int socketCliente = *(int *)arg;
	free(arg);

	char buffer[TAM_BUFFER];
	int bytesLidos;

	printf(" >> CLIENTE CONECTADO << \n");
	
	while(flag)
	{
		memset(buffer, 0, TAM_BUFFER);
		int bytesLidos = recv(socketCliente, buffer, TAM_BUFFER, 0);
		if(bytesLidos <=0)
		{
			printf(" >> CLIENTE DESCONECTADO << \n");
			flag = 0;
		}

		buffer[bytesLidos]='\0';

		if(strcmp(buffer, "sair")==0)
		{
			printf(" >> SESSÃO ENCERRADA PELO CLIENTE <<\n");
			flag = 0;
		}

		printf(" >> EXECUTANDO COMANDO << \n");
		FILE *saida = popen(buffer, "r");
		if(saida==NULL)
		{
			char *msg = " >> ERRO NA EXECUÇÃO << \n";
			send(socketCliente, msg, strlen(msg), 0);
			continue;
		}

		char output[TAM_BUFFER];
		while(fgets(output, sizeof(output), saida)!=NULL)
		{
			send(socketCliente, output, strlen(output), 0);
		}

		pclose(saida);

		char *marcadorFim = " >> FIM DA SAÍDA << \n";
		send(socketCliente, marcadorFim, strlen(marcadorFim), 0);
	}

	close(socketCliente);
	return NULL;


}
int main()
{
	int socketServidor, *socketCliente;
	struct sockaddr_in endServidor, endCliente;
	socklen_t tamEnd=sizeof(endCliente);

	socketServidor=socket(AF_INET, SOCK_STREAM, 0);
	if(socketServidor<0)
	{
		perror(" >> ERRO AO CRIAR O SERVIDOR << \n");
		exit(1);
	}

	endServidor.sin_family = AF_INET;
	endServidor.sin_port = htons(PORT);
	endServidor.sin_addr.s_addr = INADDR_ANY;

	if(bind(socketServidor, (struct sockaddr*)&endServidor, sizeof(endServidor))<0)
	{
		perror(" >> ERRO NO BIND << \n");
		exit(1);
	}

	if(listen(socketServidor, 5)<0)
	{
		perror(" >> ERRO NO LISTEN << \n");
		exit(1);
	}

	printf(" >> SERVIDOR AGUARDANDO CONEXÃO NA PORTA %d... \n", PORT);
	
	while(1)
	{
		socketCliente = malloc(sizeof(int));
		*socketCliente = accept(socketServidor, (struct sockaddr*)&endCliente, &tamEnd);
		if(*socketCliente <0)
		{
			perror(" >> ERRO NO ACCEPT << \n");
			free(socketCliente);
			continue;
		}

		pthread_t idThread;
		pthread_create(&idThread, NULL, handle_client, socketCliente);
		pthread_detach(idThread);
	}


	close(socketServidor);

	return 0;

}
