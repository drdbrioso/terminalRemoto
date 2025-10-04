#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netinet/in.h>

#define PORT 8080 
#define TAM_BUFFER 1024

// Função que gerencia cada cliente separadamente 
void *gerenciaCliente(void *arg)
{

	// Variáveis
	int flag=1;
	char buffer[TAM_BUFFER];

	// Obtém o socket do cliente e libera a memória alocada para o socket
	int socketCliente = *(int *)arg;
	free(arg);

	
	// Caso tenha chegado até aqui, o cliente está conectado
	printf(" >> CLIENTE CONECTADO << \n");
	
	// Loop que realiza comunicação de servidor com cliente e verifica possíveis erros
	while(flag)
	{
		// Inicializa o buffer com valor 0
		memset(buffer, 0, TAM_BUFFER);									
		
		// Recebe os dados pelo cliente e armazena-os em "buffer"
		int bytesLidos = recv(socketCliente, buffer, TAM_BUFFER, 0);
		
		// Termina de formatar "buffer"
		buffer[bytesLidos]='\0';

		// Verifica se a conexão foi encerrada ou se há algum erro
		if(bytesLidos <=0)												
		{
			printf(" >> CLIENTE DESCONECTADO << \n");
			flag = 0;
		}

		// Verifica se a mensagem enviada foi o comando de saída e sai se for
		if(strcmp(buffer, "sair")==0)										
		{
			printf(" >> SESSÃO ENCERRADA PELO CLIENTE <<\n");
			flag = 0;
		}
		
		// Executa o comando enviado pelo cliente
		printf(" >> EXECUTANDO COMANDO << \n");
		
		// popen = executa o comando escrito em "buffer" como em um terminal e armazena a saída em "*saida"
		FILE *saida = popen(buffer, "r");									
	
		// Verifica se a saída é válida e envia a mensagem de erro ao cliente caso não seja
		if(saida==NULL)														
		{
			char *msg = " >> ERRO NA EXECUÇÃO << \n";
			send(socketCliente, msg, strlen(msg), 0);
			continue;
		}
		
		// Envia o output do comando para o cliente
		char output[TAM_BUFFER];
		while(fgets(output, sizeof(output), saida)!=NULL)					
		{
			send(socketCliente, output, strlen(output), 0);
		}
		
		// Fecha o arquivo de saída
		pclose(saida);
		
		// Define o marcador de fim da saída e envia ao cliente. Com o fim de uma saída marcado, é possível executar outro comando
		char *marcadorFim = "<END>";
		send(socketCliente, marcadorFim, strlen(marcadorFim), 0);		
	}
	
	// Fecha o socket do cliente ao fim da conexão
	close(socketCliente);

	return NULL;
}

int main()
{

	// Variáveis
	int socketServidor, *socketCliente;
	struct sockaddr_in endServidor, endCliente;
	socklen_t tamEnd=sizeof(endCliente);
	
	// Cria o socket do servidor
	//		- AF_INET = endereços IPv4
	//		- SOCK_STREAM = protocolo baseado em conexão (TCP)
	socketServidor=socket(AF_INET, SOCK_STREAM, 0);

	// Verifica se a criação do socket foi bem sucedida e envia mensagem de erro caso não tenha
	if(socketServidor<0)													
	{
		perror(" >> ERRO AO CRIAR O SERVIDOR << \n");
		exit(1);
	}


	// Define as informações do servidor
	//		- AF_INET = endereços IPv4
	//		- PORT = 8080
	//		- INADDR_ANY = faz o bind com qualquer interface de rede do host
	endServidor.sin_family = AF_INET;
	endServidor.sin_port = htons(PORT);
	endServidor.sin_addr.s_addr = INADDR_ANY;
	
	// Realiza o bind, associando o socket ao endereço do servidor, e verifica se houve erro. Caso haja, envia mensagem de erro
	if(bind(socketServidor, (struct sockaddr*)&endServidor, sizeof(endServidor))<0)		
	{
		perror(" >> ERRO NO BIND << \n");
		exit(1);
	}
	
	// Inicia o listen e verifica se houve erro. Caso haja, envia mensagem de erro
	// 5 é o tamanho máximo da fila de conexões pendentes 
	if(listen(socketServidor, 5)<0)														
	{
		perror(" >> ERRO NO LISTEN << \n");
		exit(1);
	}
	
	// Caso não tenha encontrado erros, começa a esperar por conexões de clientes
	// Loop de conexão
	printf(" >> SERVIDOR AGUARDANDO CONEXÃO NA PORTA %d... \n", PORT);
	while(1)
	{
		// Cria o socket do cliente
		socketCliente = malloc(sizeof(int));

		// Espera pela conexão do cliente na porta 8080, onde o socket do servidor permanece no modo de escuta
		*socketCliente = accept(socketServidor, (struct sockaddr*)&endCliente, &tamEnd);	
		
		// Checa se houve erro no accept e envia mensagem de erro e libera o socket do cliente caso haja
		if(*socketCliente <0)																
		{
			perror(" >> ERRO NO ACCEPT << \n");
			free(socketCliente);
			continue;
		}
		
		// Cria a thread para o cliente utilizando a função "gerenciaCliente" utilizando o socket do cliente
		// Garante que clientes terão suas interações com o servidor isoladamente
		pthread_t idThread;
		pthread_create(&idThread, NULL, gerenciaCliente, socketCliente);

		// Libera os recursos do cliente após a conexão
		pthread_detach(idThread);
	}

	// Fecha o socket do servidor ao fim da conexão
	close(socketServidor);

	return 0;
}
