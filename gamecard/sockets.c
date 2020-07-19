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

	if (connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1) {
		return ERROR;
	}

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

void establecerConexionBroker() {
    suscribeNew = getSuscribe(NEW_POKEMON);
    suscribeCatch = getSuscribe(CATCH_POKEMON);
    suscribeGet = getSuscribe(GET_POKEMON);

    if (suscribeNew->conexion != -1) {
		printf("Conexion: %i\n", suscribeNew->conexion);
        printf("Se establecio una conexion con el Broker :D\n");

    	if(pthread_create(&threadSUSCRIBE_NEW,NULL,(void*)suscribe,(void*)suscribeNew) != 0)
        printf("Error");
        
		if(pthread_create(&threadSUSCRIBE_CATCH,NULL,(void*)suscribe,(void*)suscribeCatch) != 0)
        printf("Error");

   		if(pthread_create(&threadSUSCRIBE_GET,NULL,(void*)suscribe,(void*)suscribeGet) != 0)
        printf("Error");

        pthread_detach(threadSUSCRIBE_NEW);
        pthread_detach(threadSUSCRIBE_CATCH);
        pthread_detach(threadSUSCRIBE_GET);
    }
    else {
        printf("No se pudo establecer una conexion con el Broker :c\n");
        printf("Intentando reconexion en %i segundos...\n", (uint32_t)config_get_int_value(config,"TIEMPO_DE_REINTENTO_CONEXION"));
        sem_post(&mutexReconnect);
    }
}

void reconectarBroker() {
	while(1) {
		sem_wait(&mutexReconnect);
		sleep((uint32_t)config_get_int_value(config,"TIEMPO_DE_REINTENTO_CONEXION"));
		printf("Intentando reconexion con Broker...\n");
		establecerConexionBroker();
	}
}

void detectarDesconexion() {
	while(1) {
		sem_wait(&detectorDesconexion);
		sem_wait(&detectorDesconexion);
		sem_wait(&detectorDesconexion);
		printf("Se desconecto el Broker :c\n");

		sem_post(&mutexReconnect);
	}
}



void suscribe(void* structSuscribe) {
	t_suscribe* s = (t_suscribe*) structSuscribe;

	t_register_module* suscribe = crearSuscribe(s->messageQueue, idModule);
	t_paquete* paquete = serializar_registerModule(suscribe);
	printf("SUSCRIBE %i \n", s->messageQueue);
	free(suscribe);
	enviarMensaje(paquete, s->conexion);
	
	while(1) {
		serve_suscribe(&s->conexion);
	}
}

void enviarAck(t_acknowledgement* ack) {
	char* ip = config_get_string_value(config, "IP_BROKER");
    char* puerto = config_get_string_value(config, "PUERTO_BROKER");
	t_paquete* paquete = serializar_acknowledgement(ack);
	free(ack);
	uint32_t respuestaBroker = crear_conexion(ip, puerto);
	enviarMensaje(paquete, respuestaBroker);
	liberar_conexion(respuestaBroker);
	return;
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
	pthread_detach(thread);
}

void serve_client(uint32_t *socket)
{
	uint32_t cod_op;
	t_buffer* buffer;
	if (recv(*socket, &cod_op, sizeof(int), MSG_WAITALL) == -1)
		cod_op = -1;
	if(cod_op > 0 && cod_op < 11) {
		buffer = recibir_buffer(*socket);
	}
	process_request(cod_op, buffer, *socket);
}

void process_request(uint32_t cod_op, t_buffer* buffer, uint32_t cliente_fd)
{
	log_info(logger,"atendiendo al cliente %i\n", cliente_fd);
	switch (cod_op)
	{
		case NEW_POKEMON:
		{
			log_info(logger, "SIZE BUFFER EN NEW: %i\n", buffer->size);
			t_new_pokemon *newPoke = deserializar_newPokemon(buffer);
			printf("Nombre del poke new: %s\n", newPoke->nombre);
			t_acknowledgement* ack = crearAcknowledgement(idModule, newPoke->ID_mensaje_recibido, NEW_POKEMON);
			t_posicion* posicion = crearPosicion(newPoke->posicionCantidad->posicion_x, newPoke->posicionCantidad->posicion_y);
			t_appeared_pokemon* appearedPokemon = crearAppearedPokemon(0, newPoke->ID_mensaje_recibido, newPoke->nombre, posicion);
				
			pthread_t sendAck;
			pthread_create(&sendAck, NULL, (void*)enviarAck, ack);
			pthread_detach(sendAck);
			pthread_t hiloAtenderNewPoke;
			pthread_create(&hiloAtenderNewPoke, NULL, atenderNewPokemon,(void*)newPoke);
			pthread_detach(hiloAtenderNewPoke);
			log_info(logger, newPoke->nombre);
			//podria generar la conexion con broker desde un principio, por donde se le va a enviar los appeared, caught y localized ,pero que sea el servidor que reciba las requests
			//enviar a broker
			
			// free(newPoke);

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

			t_caught_pokemon* caughtPoke = crearCaughtPokemon(0,catchPoke->ID_mensaje_recibido,1);

			t_acknowledgement* ack = crearAcknowledgement(idModule, catchPoke->ID_mensaje_recibido, CATCH_POKEMON);

			pthread_t sendAck;
			pthread_create(&sendAck, NULL, (void*)enviarAck, ack);
			pthread_detach(sendAck);
			pthread_t hiloAtenderCatchPoke;
			pthread_create(&hiloAtenderCatchPoke, NULL, (void*)atenderCatchPokemon,(void*)catchPoke);
			pthread_detach(hiloAtenderCatchPoke);

			printf("Nombre del poke a atrapar: %s\n", catchPoke->nombre);
			//enviar a broker

			log_info(logger, catchPoke->nombre);
			// free(catchPoke);
		
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

void serve_suscribe(uint32_t* socket)
{
	int cod_op;
	t_buffer* buffer;
	if(recv(*socket, &cod_op, sizeof(int), MSG_WAITALL) == -1)
		cod_op = -1;
	if(cod_op > 0 && cod_op < 11) {
		buffer = recibir_buffer(*socket);
	}
	process_suscribe_request(cod_op, buffer, *socket);
}

void process_suscribe_request(uint32_t cod_op, t_buffer* buffer, uint32_t cliente_fd) {
	switch (cod_op) {
		case NEW_POKEMON:
		{
			log_info(logger, "SIZE BUFFER EN NEW: %i\n", buffer->size);
			t_new_pokemon *newPoke = deserializar_newPokemon(buffer);
			printf("Nombre del poke new: %s\n", newPoke->nombre);
			t_acknowledgement* ack = crearAcknowledgement(idModule,newPoke->ID_mensaje_recibido, NEW_POKEMON);
			t_posicion* posicion = crearPosicion(newPoke->posicionCantidad->posicion_x, newPoke->posicionCantidad->posicion_y);
			t_appeared_pokemon* appearedPokemon = crearAppearedPokemon(0, newPoke->ID_mensaje_recibido, newPoke->nombre, posicion);
				
			pthread_t sendAck;
			pthread_create(&sendAck, NULL, (void*)enviarAck, ack);
			pthread_detach(sendAck);
			pthread_t hiloAtenderNewPoke;
			pthread_create(&hiloAtenderNewPoke, NULL, atenderNewPokemon,(void*)newPoke);
			pthread_detach(hiloAtenderNewPoke);

			
			log_info(logger, newPoke->nombre);

			free(buffer->stream);
			free(buffer);
			break;
		}
		case CATCH_POKEMON:
		{
			log_info(logger, "SIZE BUFFER EN NEW: %i\n", buffer->size);
			t_catch_pokemon *catchPoke = deserializar_catchPokemon(buffer);

			t_caught_pokemon* caughtPoke = crearCaughtPokemon(0,catchPoke->ID_mensaje_recibido,1);

			t_acknowledgement* ack = crearAcknowledgement(idModule ,catchPoke->ID_mensaje_recibido, CATCH_POKEMON);

			pthread_t sendAck;
			pthread_create(&sendAck, NULL, (void*)enviarAck, ack);
			pthread_detach(sendAck);
			pthread_t hiloAtenderCatchPoke;
			pthread_create(&hiloAtenderCatchPoke, NULL, (void*)atenderCatchPokemon,(void*)catchPoke);
			pthread_detach(hiloAtenderCatchPoke);

			printf("Nombre del poke a atrapar: %s\n", catchPoke->nombre);
			//enviar a broker

			log_info(logger, catchPoke->nombre);
			// free(catchPoke);
		
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
		default:
		{
			printf("Se corto la conexion\n");
			sem_post(&detectorDesconexion);
			pthread_exit(&threadSUSCRIBE_NEW);
			pthread_exit(&threadSUSCRIBE_CATCH);
			pthread_exit(&threadSUSCRIBE_GET);
			break;
		}
	}
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

uint32_t escuchaBroker(){
	char* IP_BROKER = config_get_string_value(config,"IP_BROKER");
    char* PUERTO_BROKER = config_get_string_value(config,"PUERTO_BROKER");
	uint32_t conexion = crear_conexion(IP_BROKER, PUERTO_BROKER);
	printf("CONEXION: %i\n", conexion);

	return conexion;
}



