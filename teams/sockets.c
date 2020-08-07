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

	if (connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1)
	{
		return ERROR;
	}

	freeaddrinfo(server_info);

	return socket_cliente;
}

void liberar_conexion(uint32_t socket_cliente)
{
	close(socket_cliente);
}

void desconectarBroker(uint32_t mq)
{
	t_register_module *reg = crearSuscribe(mq, idModule);
	t_paquete *paquete = serializar_desconexion(reg);

	uint32_t con = escuchaBroker();

	enviarMensaje(paquete, con);
	liberar_conexion(con);
}

void establecerConexionBroker()
{
	suscribeCaught = getSuscribe(CAUGHT_POKEMON);
	suscribeAppeared = getSuscribe(APPEARED_POKEMON);
	suscribeLocalized = getSuscribe(LOCALIZED_POKEMON);

	if (suscribeAppeared->conexion != -1)
	{
		log_info(logger, "Se establecio una conexion con el Broker :D");
		log_info(logger, "Desactivando modo default \n");
		int valorSem;
		if (sem_getvalue(&estaDesconectado, &valorSem) == 0)
		{
			if (valorSem != 0)
			{
				sem_wait(&estaDesconectado);
			}
		}
		mandarGET();

		pthread_create(&threadSUSCRIBE_CAUGHT, NULL, (void *)suscribe, (void *)suscribeCaught);
		pthread_create(&threadSUSCRIBE_APPEARED, NULL, (void *)suscribe, (void *)suscribeAppeared);
		pthread_create(&threadSUSCRIBE_LOCALIZED, NULL, (void *)suscribe, (void *)suscribeLocalized);

		pthread_detach(threadSUSCRIBE_CAUGHT);
		pthread_detach(threadSUSCRIBE_APPEARED);
		pthread_detach(threadSUSCRIBE_LOCALIZED);
	}
	else
	{
		log_info(logger, "No se pudo establecer una conexion con el Broker :c\n");
		int valorSem;
		if (sem_getvalue(&estaDesconectado, &valorSem) == 0)
		{
			if (valorSem == 0)
			{
				log_info(logger, "Activando modo default");
				modoDesconectado();
				sem_post(&estaDesconectado);
			}
		}
		sem_post(&mutexReconnect);
	}
}

void reconectarBroker()
{
	while (1)
	{
		sem_wait(&mutexReconnect);
		sleep((uint32_t)config_get_int_value(config, "TIEMPO_RECONEXION"));
		log_info(logger, "Intentando reconexion con Broker...\n");
		establecerConexionBroker();
	}
}

void detectarDesconexion()
{
	while (1)
	{
		sem_wait(&detectorDesconexion);
		sem_wait(&detectorDesconexion);
		sem_wait(&detectorDesconexion);
		log_info(logger, "Se desconecto el Broker :c\n");
		log_info(logger, "Activando modo default \n");
		modoDesconectado();
		sem_post(&estaDesconectado);
		sem_post(&mutexReconnect);
	}
}

void enviarMensaje(t_paquete *paquete, uint32_t socket_cliente)
{
	uint32_t sizePaquete = paquete->buffer->size + 2 * sizeof(uint32_t);
	void *stream = serializar_paquete(paquete, sizePaquete);
	send(socket_cliente, stream, sizePaquete, 0);
	liberarPaquete(paquete);
	free(stream);
}

void mandarGET()
{

	char *ip = config_get_string_value(config, "IP_BROKER");
	char *puerto = config_get_string_value(config, "PUERTO_BROKER");
	//uint32_t conexion = crear_conexion(ip, puerto);

	for (uint32_t i = 0; i < list_size(objetivoGlobal); i++)
	{
		uint32_t conexion = crear_conexion(ip, puerto);
		t_pokemon_cantidad *poke = (t_pokemon_cantidad *)list_get(objetivoGlobal, i);
		if (poke->cantidad > 0)
		{
			//sendGET(poke->nombre);
			t_get_pokemon *getPokemon = crearGetPokemon(11, poke->nombre);
			t_paquete *paquete = serializar_getPokemon(getPokemon);
			log_info(logger, "Mandando GET %s\n", poke->nombre);
			free(getPokemon);
			enviarMensaje(paquete, conexion);
			liberar_conexion(conexion);
		}
	}
	//liberar_conexion(conexion);
}

void suscribe(void *structSuscribe)
{
	t_suscribe *s = (t_suscribe *)structSuscribe;

	t_register_module *suscribe = crearSuscribe(s->messageQueue, idModule);
	t_paquete *paquete = serializar_registerModule(suscribe);
	log_info(logger, "SUSCRIBE %i \n", s->messageQueue);
	free(suscribe);
	enviarMensaje(paquete, s->conexion);

	while (1)
	{
		serve_suscribe(&s->conexion);
	}
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

	int cod_op;
	t_buffer *buffer;
	if (recv(*socket, &cod_op, sizeof(int), MSG_WAITALL) == -1)
		cod_op = -1;
	if (cod_op > 0 && cod_op < 11)
	{
		buffer = recibir_buffer(*socket);
	}
	process_request(cod_op, buffer, *socket);
}

void process_request(uint32_t cod_op, t_buffer *buffer, uint32_t cliente_fd)
{
	switch (cod_op)
	{
	case APPEARED_POKEMON:
	{
		//log_info(logger, "SIZE BUFFER EN NEW: %i", buffer->size);
		// deserializar_appearedPokemon(buffer);
		t_appeared_pokemon *appearedPoke = deserializar_appearedPokemon(buffer);
		t_pokemon_posicion *poke = crearPokemonPosicion(appearedPoke->nombre, appearedPoke->posicion);
		log_info(logger, "APPEARED: Aparecio un %s salvaje en x:%i y:%i", poke->nombre, poke->posicion->posicion_x, poke->posicion->posicion_y);
		insertPokeEnMapa(poke);

		free(appearedPoke);
		free(buffer->stream);
		free(buffer);
		break;
	}
	case CAUGHT_POKEMON:
	{
		t_caught_pokemon *caughtPoke = deserializar_caughtPokemon(buffer);
		if (caughtPoke->catchStatus == 1)
		{
			log_info(logger, "CAUGHT: Llego para el mensaje %i que la captura fue exitosa", caughtPoke->ID_mensaje_original);
		}
		else
		{
			log_info(logger, "CAUGHT: Llego para el mensaje %i que la captura no fue exitosa", caughtPoke->ID_mensaje_original);
		}
		procesarMensajeCaught(caughtPoke);
		free(buffer->stream);
		free(buffer);
		break;
	}
	case LOCALIZED_POKEMON:
	{
		//log_info(logger, "SIZE BUFFER EN NEW: %i", buffer->size);
		// deserializar_appearedPokemon(buffer);
		t_localized_pokemon *localizedPoke = deserializar_localizedPokemon(buffer);

		log_info(logger, "LOCALIZED: Llego %s", localizedPoke->nombre);
		for (uint32_t i = 0; i < localizedPoke->cantidadPosiciones; i++)
		{
			t_pokemon_posicion *poke = crearPokemonPosicion(localizedPoke->nombre, (t_posicion *)list_get(localizedPoke->posiciones, i));
			insertPokeEnMapa(poke);
			log_info(logger, " - X: %i, Y: %i", ((t_posicion *)list_get(localizedPoke->posiciones, i))->posicion_x, ((t_posicion *)list_get(localizedPoke->posiciones, i))->posicion_y);
		}

		free(localizedPoke);
		free(buffer->stream);
		free(buffer);
		break;
	}
	case 0:
		pthread_exit(NULL);
	case -1:
		pthread_exit(NULL);
	}
}

void serve_suscribe(uint32_t *socket)
{
	int cod_op;
	t_buffer *buffer;
	if (recv(*socket, &cod_op, sizeof(int), MSG_WAITALL) == -1)
		cod_op = -1;
	if (cod_op > 0 && cod_op < 11)
	{
		buffer = recibir_buffer(*socket);
		if (buffer->size > 1000 || buffer->size < 0)
		{
			cod_op = -1;
			free(buffer->stream);
			free(buffer);
		}
	}
	process_suscribe_request(cod_op, buffer, *socket);
}

void process_suscribe_request(uint32_t cod_op, t_buffer *buffer, uint32_t cliente_fd)
{
	switch (cod_op)
	{
	case APPEARED_POKEMON:
	{
		//log_info(logger, "SIZE BUFFER EN NEW: %i", buffer->size);
		// deserializar_appearedPokemon(buffer);
		t_appeared_pokemon *appearedPoke = deserializar_appearedPokemon(buffer);
		t_pokemon_posicion *poke = crearPokemonPosicion(appearedPoke->nombre, appearedPoke->posicion);
		log_info(logger, "APPEARED: Aparecio un %s salvaje en x:%i y:%i", poke->nombre, poke->posicion->posicion_x, poke->posicion->posicion_y);
		insertPokeEnMapa(poke);
		t_acknowledgement *ack = crearAcknowledgement(idModule, appearedPoke->ID_mensaje_recibido, APPEARED_POKEMON);
		enviarAck(ack);
		free(appearedPoke);
		free(buffer->stream);
		free(buffer);
		break;
	}
	case CAUGHT_POKEMON:
	{
		t_caught_pokemon *caughtPoke = deserializar_caughtPokemon(buffer);
		if (caughtPoke->catchStatus == 1)
		{
			log_info(logger, "CAUGHT: Llego para el mensaje %i que la captura fue exitosa", caughtPoke->ID_mensaje_original);
		}
		else
		{
			log_info(logger, "CAUGHT: Llego para el mensaje %i que la captura no fue exitosa", caughtPoke->ID_mensaje_original);
		}
		procesarMensajeCaught(caughtPoke);
		t_acknowledgement *ack = crearAcknowledgement(idModule, caughtPoke->ID_mensaje_recibido, CAUGHT_POKEMON);
		enviarAck(ack);
		free(buffer->stream);
		free(buffer);
		break;
	}
	case LOCALIZED_POKEMON:
	{
		//log_info(logger, "SIZE BUFFER EN NEW: %i", buffer->size);
		// deserializar_appearedPokemon(buffer);
		t_localized_pokemon *localizedPoke = deserializar_localizedPokemon(buffer);

		if (localizedPoke->cantidadPosiciones > 0)
		{
			log_info(logger, "LOCALIZED: Llego %s", localizedPoke->nombre);

			for (uint32_t i = 0; i < localizedPoke->cantidadPosiciones; i++)
			{
				t_pokemon_posicion *poke = crearPokemonPosicion(localizedPoke->nombre, (t_posicion *)list_get(localizedPoke->posiciones, i));
				insertPokeEnMapa(poke);
				log_info(logger, " - X: %i, Y: %i", ((t_posicion *)list_get(localizedPoke->posiciones, i))->posicion_x, ((t_posicion *)list_get(localizedPoke->posiciones, i))->posicion_y);
			}
		}
		else
		{
			log_info(logger, "LOCALIZED: Llego %s sin posiciones", localizedPoke->nombre);
		}

		t_acknowledgement *ack = crearAcknowledgement(idModule, localizedPoke->ID_mensaje_recibido, LOCALIZED_POKEMON);
		enviarAck(ack);
		free(localizedPoke);
		free(buffer->stream);
		free(buffer);
		break;
	}
	default:
	{
		sem_post(&detectorDesconexion);
		pthread_exit(&threadSUSCRIBE_CAUGHT);
		pthread_exit(&threadSUSCRIBE_APPEARED);
		pthread_exit(&threadSUSCRIBE_LOCALIZED);
		break;
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

uint32_t escuchaBroker()
{
	char *IP_BROKER = config_get_string_value(config, "IP_BROKER");
	char *PUERTO_BROKER = config_get_string_value(config, "PUERTO_BROKER");

	return crear_conexion(IP_BROKER, PUERTO_BROKER);
}

void enviarAck(t_acknowledgement *ack)
{
	t_paquete *paquete = serializar_acknowledgement(ack);
	free(ack);
	uint32_t conexion = escuchaBroker();
	enviarMensaje(paquete, conexion);
	liberar_conexion(conexion);
}