#include "sockets.h"

/*CLIENT SIDE*/

int crear_conexion(char *ip, char *puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	uint32_t socket_cliente = socket(server_info->ai_family,
									 server_info->ai_socktype, server_info->ai_protocol);

	// int resultadoConexion = connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);

	// // if (connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1)
	// // 	printf("error");

	// freeaddrinfo(server_info);
	// if(resultadoConexion == ERROR){
	// 	return -1;
	// }
	// 		server_info->ai_socktype, server_info->ai_protocol);

	if (connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen)
			== -1)
		printf("error");

	freeaddrinfo(server_info);

	return socket_cliente;
}

void liberar_conexion(uint32_t socket_cliente)
{
	close(socket_cliente);
}

void enviarMensaje(t_paquete *paquete, uint32_t socket_cliente)
{
	uint32_t sizePaquete = paquete->buffer->size + 2 * sizeof(uint32_t);
	void* stream = serializar_paquete(paquete, sizePaquete);
	send(socket_cliente,stream,sizePaquete,0);
	liberarPaquete(paquete);
	free(stream);
}

/*SERVER SIDE*/

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

	while (1)
		esperar_cliente(socket_servidor);
}

void esperar_cliente(uint32_t socket_servidor)
{
	struct sockaddr_in dir_cliente;

	uint32_t tam_direccion = sizeof(struct sockaddr_in);

	uint32_t socket_cliente = accept(socket_servidor, (void *)&dir_cliente, &tam_direccion);

	pthread_create(&thread, NULL, (void *)serve_client, &socket_cliente);
	pthread_join(thread, NULL);
}

void serve_client(uint32_t *socket)
{
	uint32_t cod_op;
	if (recv(*socket, &cod_op, sizeof(int), MSG_WAITALL) == -1)
		cod_op = -1;
	printf("\nCodigo de op: %i\n", cod_op);
	process_request(cod_op, *socket);
}

void process_request(uint32_t cod_op, uint32_t cliente_fd)
{
	log_info(logger,"atendiendo al cliente %i", cliente_fd);
	t_buffer *buffer = recibir_buffer(cliente_fd);
	switch (cod_op)
	{
	case CONFIRMACION_MSJ:
	{
		log_info(logger, "SIZE BUFFER EN CONFIRMACION MENSAJE: %i\n", buffer->size);
		t_confirmacion_mensaje* confirmacion = deserializar_confirmacionMensaje(buffer);
		printf("llego la confirmacion de mensaje del suscriptor: %i, del mensaje de ID: %i, para la cola: %i, con el resultado: %i\n", cliente_fd, confirmacion->ID_mensaje, confirmacion->MessageQueue, confirmacion->meLlego);
		free(confirmacion);
		// free(buffer->stream);
		// free(buffer);
		break;
	}
	case NEW_POKEMON:
	{
		log_info(logger, "SIZE BUFFER EN NEW: %i\n", buffer->size);
		t_new_pokemon *newPoke = deserializar_newPokemon(buffer);
		printf("Nombre del poke new: %s\n", newPoke->nombre);
		pthread_t hiloAtenderPoke;
		pthread_create(&hiloAtenderPoke, NULL, atenderNewPokemon,(void*)newPoke);
    	pthread_join(hiloAtenderPoke,NULL);
		// atenderNewPokemon(newPoke);
		log_info(logger, newPoke->nombre);
		t_posicion* posicion = crearPosicion(newPoke->posicionCantidad->posicion_x, newPoke->posicionCantidad->posicion_y);
		t_appeared_pokemon* appearedPokemon = crearAppearedPokemon(0, newPoke->ID_mensaje_recibido, newPoke->nombre, posicion);
		//podria generar la conexion con broker desde un principio, por donde se le va a enviar los appeared, caught y localized ,pero que sea el servidor que reciba las requests
		//enviar a broker
		
		free(newPoke);

		break;
	}
	case APPEARED_POKEMON:
	{
		log_info(logger, "SIZE BUFFER EN NEW: %i", buffer->size);
		// deserializar_appearedPokemon(buffer);
		t_appeared_pokemon *appearedPoke = deserializar_appearedPokemon(buffer);
		printf("pos x de poke: %i", appearedPoke->posicion->posicion_x);
		log_info(logger, appearedPoke->nombre);

		free(appearedPoke);
		// free(buffer->stream);
		// free(buffer);
		break;
	}
	case CATCH_POKEMON:
	{
		log_info(logger, "SIZE BUFFER EN NEW: %i\n", buffer->size);
		t_catch_pokemon *catchPoke = deserializar_catchPokemon(buffer);
		printf("Nombre del poke a atrapar: %s\n", catchPoke->nombre);
		atenderCatchPokemon(catchPoke);
		t_caught_pokemon* caughtPoke = crearCaughtPokemon(0,catchPoke->ID_mensaje_recibido,1);
		//enviar a broker

		log_info(logger, catchPoke->nombre);
		free(catchPoke);
	
		break;
	}
	case CAUGHT_POKEMON:
	{


		
		break;
	}
	case GET_POKEMON:
	{
		t_get_pokemon *getPoke = deserializar_getPokemon(buffer);
		log_info(logger, getPoke->nombre);
		t_localized_pokemon* localizedPoke = atenderGetPokemon(getPoke);
		
		
		//enviar a broker


		free(getPoke);
		
		break;
	}
	case 0:
		pthread_exit(NULL);
	case -1:
		pthread_exit(NULL);
	}
	free(buffer->stream);
	free(buffer);
}

t_buffer *recibir_buffer(uint32_t socket_cliente)
{
	t_buffer *buffer = malloc(sizeof(t_buffer));
	int size;

	recv(socket_cliente, &size, sizeof(int), MSG_WAITALL);
	printf("SOcket cliente: %i\n", socket_cliente);
	printf("Buffer size: %d\n", size);
	buffer->size = size;
	buffer->stream = malloc(buffer->size);
	recv(socket_cliente, buffer->stream, buffer->size, MSG_WAITALL);

	return buffer;
}

//Devuelve la cola de mensajes dado un id


uint32_t escuchaBroker(){
	char* IP_BROKER = config_get_string_value(config,"IP_BROKER");
    char* PUERTO_BROKER = config_get_string_value(config,"PUERTO_BROKER");

	return crear_conexion(IP_BROKER, PUERTO_BROKER);
}

// recv(socket_cliente, &size, sizeof(int), MSG_WAITALL);
// 		mensaje = malloc(size);
// 		recv(socket_cliente, mensaje, size, MSG_WAITALL);
// void* recibir_buffer(uint32_t* size, uint32_t socket_cliente)
// {
// 	void * buffer;

// 	recv(socket_cliente, size, sizeof(uint32_t), MSG_WAITALL);
// 	buffer = malloc(*size);
// 	recv(socket_cliente, buffer, *size, MSG_WAITALL);

// 	return buffer;
// }
