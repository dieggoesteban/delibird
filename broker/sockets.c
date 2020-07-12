#include "sockets.h"

#pragma region Funciones de Cliente

void enviarMensaje(t_paquete* paquete, uint32_t socket_cliente) {
	int sizePaquete = paquete->buffer->size + 2 * sizeof(int);
	void* stream = serializar_paquete(paquete, sizePaquete);
	send(socket_cliente,stream,sizePaquete,MSG_CONFIRM);
	liberarPaquete(paquete);
	free(stream);
}
void liberarPaquete(t_paquete* paquete){
	free(paquete->buffer->stream);
	free(paquete->buffer);	
	free(paquete);
}

#pragma endregion

#pragma region Funciones de Servidor
void iniciar_servidor(void)
{
	uint32_t socket_servidor;
	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(IP, PUERTO, &hints, &servinfo);

	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
			continue;

		if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(socket_servidor);
			continue;
		}
		break;
	}

	listen(socket_servidor, SOMAXCONN);
	freeaddrinfo(servinfo);

	while (true)
		esperar_cliente(socket_servidor);
}

void esperar_cliente(uint32_t socket_servidor)
{
	struct sockaddr_in dir_cliente;

	uint32_t tam_direccion = sizeof(struct sockaddr_in);
	uint32_t socket_cliente = accept(socket_servidor, (void *)&dir_cliente, &tam_direccion);
	pthread_create(&serverThread, NULL, (void *)serve_client, &socket_cliente);
	pthread_detach(serverThread);
}

void serve_client(uint32_t *socket_cliente)
{
	uint32_t operation_cod;
	if (recv(*socket_cliente, &operation_cod, sizeof(int), MSG_WAITALL) == -1)
		operation_cod = -1;
	printf("\nCodigo de operacion: %i\n", operation_cod);
	process_request(operation_cod, *socket_cliente);
}

void process_request(uint32_t operation_cod, uint32_t socket_cliente)
{
    t_buffer *buffer = recibir_buffer(socket_cliente);
	processMessage(buffer, operation_cod, socket_cliente);
}

t_buffer *recibir_buffer(uint32_t socket_cliente)
{
	t_buffer *buffer = malloc(sizeof(t_buffer));
	int size = 0;

	recv(socket_cliente, &size, sizeof(int), MSG_WAITALL);
	buffer->size = size;
	buffer->stream = malloc(buffer->size);
	recv(socket_cliente, buffer->stream, buffer->size, MSG_WAITALL);

	return buffer;
}
#pragma endregion
