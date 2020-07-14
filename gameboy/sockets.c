#include "sockets.h"

int crear_conexion(char *ip, char* puerto) {
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family,
			server_info->ai_socktype, server_info->ai_protocol);

	if (connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen)
			== -1)
		printf("error");

	freeaddrinfo(server_info);

	return socket_cliente;
}

void liberar_conexion(int socket_cliente) {
	close(socket_cliente);
}

void enviarMensaje(t_paquete* paquete, uint32_t socket_cliente) {
	int sizePaquete = paquete->buffer->size + 2 * sizeof(int);
	void* stream = serializar_paquete(paquete, sizePaquete);
	send(socket_cliente,stream,sizePaquete,MSG_CONFIRM);
	liberarPaquete(paquete);
	free(stream);
}

void suscribe(void* message_queue) {
	char* ip = config_get_string_value(config, "IP_BROKER");
    char* puerto = config_get_string_value(config, "PUERTO_BROKER");
    uint32_t conexion = crear_conexion(ip, puerto);
	
	uint32_t mq = (uint32_t) message_queue;
	t_register_module* suscribe = crearSuscribe(mq);
	t_paquete* paquete = serializar_registerModule(suscribe);
	log_info(logger, "SUSCRIBE %i ", mq);
	free(suscribe);
	enviarMensaje(paquete, conexion);

	while(true) {
		serve_client(&conexion);
	}
}

void serve_client(uint32_t* socket)
{
	
	uint32_t cod_op;
	if(recv(*socket, &cod_op, sizeof(int), MSG_WAITALL) == -1)
		cod_op = -1;
	t_buffer *buffer = recibir_buffer(*socket);
	process_request(buffer, cod_op, *socket);
}

void process_request(t_buffer *buffer, uint32_t operation_cod, uint32_t socket_cliente) {
	switch (operation_cod)
	{
		// case ACKNOWLEDGEMENT:
		// {
		// 	t_acknowledgement* ack = deserializar_acknowledgement(buffer);
		// 	// receiveAcknowledgement(ack, socket_cliente);
		// 	log_info(logger, "Se ha recibido ACK de %i por el mensaje %i", socket_cliente, ack->idMessageReceived);
		// }
		case NEW_POKEMON:
		{
	
			t_new_pokemon* newPoke = deserializar_newPokemon(buffer);
	
			log_info(logger, "Se ha recibido un mensaje del cliente %i del cod_op", socket_cliente, operation_cod);
			log_info(logger, "Se trajo el new_pokemon de nombre %s", newPoke->nombre);
		
			break;
		}
		case APPEARED_POKEMON:
		{

			t_appeared_pokemon* appearedPoke = deserializar_appearedPokemon(buffer);
	
			log_info(logger, "Se ha recibido un mensaje del cliente %i del cod_op", socket_cliente, operation_cod);
			log_info(logger, "Se trajo el appeared_pokemon de nombre %s", appearedPoke->nombre);

			break;
		}
		case GET_POKEMON:
		{
			t_get_pokemon* getPoke = deserializar_getPokemon(buffer);
		
			log_info(logger, "Se ha recibido un mensaje del cliente %i del cod_op", socket_cliente, operation_cod);
			log_info(logger, "Se trajo el get_pokemon de nombre %s", getPoke->nombre);

			break;
		}
		case CAUGHT_POKEMON:
		{
			t_caught_pokemon* caughtPoke = deserializar_caughtPokemon(buffer);

			log_info(logger, "Se ha recibido un mensaje del cliente %i del cod_op", socket_cliente, operation_cod);
			log_info(logger, "Se trajo el caught_pokemon de ID_Mensaje %i e ID_MensajeCorrelativo", caughtPoke->ID_mensaje_recibido, caughtPoke->ID_mensaje_original);

			break;
		}
		case LOCALIZED_POKEMON:
		{
			t_localized_pokemon* localizedPoke = deserializar_localizedPokemon(buffer);
	
			log_info(logger, "Se ha recibido un mensaje del cliente %i del cod_op", socket_cliente, operation_cod);
			log_info(logger, "Se trajo el get_pokemon de nombre %s", localizedPoke->nombre);

			break;
		}
		case CATCH_POKEMON:
		{
			t_catch_pokemon* catchPoke = deserializar_catchPokemon(buffer);

			log_info(logger, "Se ha recibido un mensaje del cliente %i del cod_op", socket_cliente, operation_cod);
			log_info(logger, "Se trajo el get_pokemon de nombre %s", catchPoke->nombre);

			break;
		}
		default:
		{
			log_error(logger, "Codigo de operacion desconocido: %i", operation_cod);
		}
	}
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