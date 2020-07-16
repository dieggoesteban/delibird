#include "sockets.h"

#pragma region Funciones de Cliente

void enviarMensaje(t_paquete* paquete, uint32_t socket_cliente) {
	int sizePaquete = paquete->buffer->size + 2 * sizeof(int);
	void* stream = serializar_paquete(paquete, sizePaquete);
	if(send(socket_cliente,stream,sizePaquete,MSG_CONFIRM) == -1)
	{
		log_error(broker_custom_logger, "Send error");
		liberar_conexion(socket_cliente);
	}
	liberarPaquete(paquete);
	free(stream);
}

void liberar_conexion(uint32_t socket_cliente) {
	close(socket_cliente);
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
	pthread_join(serverThread, NULL);
}

void serve_client(uint32_t *socket_cliente)
{
	uint32_t operation_cod;
	uint32_t recvResult = recv(*socket_cliente, &operation_cod, sizeof(int), MSG_WAITALL);
	if (recvResult == -1)
	{
		printf("Recv error\n");
		liberar_conexion(*socket_cliente);
		exit(1);
		return;
	}
	else if (recvResult == 0)
	{
		printf("El cliente %i cerro la conexion\n", *socket_cliente);
		liberar_conexion(*socket_cliente);
		return;
	}
	else
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
	int size;

	recv(socket_cliente, &size, sizeof(int), MSG_WAITALL);
	buffer->size = size;
	buffer->stream = malloc(buffer->size);
	recv(socket_cliente, buffer->stream, buffer->size, MSG_WAITALL);

	return buffer;
}
#pragma endregion
