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

void desconectarBroker(uint32_t mq) {
	t_register_module* reg = crearSuscribe(mq, idModule);
	t_paquete* paquete = serializar_desconexion(reg);

	uint32_t con = escuchaBroker();

	enviarMensaje(paquete, con);
	liberar_conexion(con);

}

void establecerConexionBroker() {
    suscribeNew = getSuscribe(NEW_POKEMON);
    suscribeCatch = getSuscribe(CATCH_POKEMON);
    suscribeGet = getSuscribe(GET_POKEMON);

    if (suscribeNew->conexion != -1) {
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
	switch (cod_op)
	{
		case NEW_POKEMON:
		{
			t_new_pokemon *newPoke = deserializar_newPokemon(buffer);
			log_info(logger, "NEW_POKEMON ingresado: %s", newPoke->nombre);

			uint32_t indexSem;

			pthread_mutex_lock(&mutexListaSemsNew);
			indexSem = getIndexSemaforo(newPoke->nombre, semaforosPokemon);
			if(indexSem == ERROR){
			 	t_semaforo_pokemon* semPoke = crearSemaforoPokemon(newPoke->nombre);
				indexSem = list_add(semaforosPokemon,semPoke);
			}
			
			t_newPokemon_indexSem* newPokeSem = crearNewPokemonIndexSem(indexSem, newPoke);
			pthread_mutex_unlock(&mutexListaSemsNew);

			pthread_t hiloAtenderNewPoke;
			pthread_create(&hiloAtenderNewPoke, NULL, atenderNewPokemon,(void*)newPokeSem);
			pthread_detach(hiloAtenderNewPoke);
			free(buffer->stream);
			free(buffer);

			break;
		}
		case CATCH_POKEMON:
		{
			t_catch_pokemon *catchPoke = deserializar_catchPokemon(buffer);
			uint32_t indexSem;

			log_info(logger, "CATCH_POKEMON ingresado: %s", catchPoke->nombre);

			pthread_mutex_lock(&mutexListaSemsCatch);
			indexSem = getIndexSemaforo(catchPoke->nombre, semaforosPokemon);

			if(indexSem == ERROR){
			 	t_semaforo_pokemon* semPoke = crearSemaforoPokemon(catchPoke->nombre);
				indexSem = list_add(semaforosPokemon,semPoke);
			}
			
			t_catchPokemon_indexSem* catchPokeSem = crearCatchPokemonIndexSem(indexSem, catchPoke);
			pthread_mutex_unlock(&mutexListaSemsCatch);

			pthread_t hiloAtenderCatchPoke;
			pthread_create(&hiloAtenderCatchPoke, NULL, (void*)atenderCatchPokemon,(void*)catchPokeSem);
			pthread_detach(hiloAtenderCatchPoke);
			free(buffer->stream);
			free(buffer);
		
			break;
		}
		case GET_POKEMON:
		{
			t_get_pokemon *getPoke = deserializar_getPokemon(buffer);
	

			log_info(logger, "GET_POKEMON ingresado: %s", getPoke->nombre);
			uint32_t indexSem;

			pthread_mutex_lock(&mutexListaSemsGet);
			indexSem = getIndexSemaforo(getPoke->nombre, semaforosPokemon);
			if(indexSem == ERROR){
			 	t_semaforo_pokemon* semPoke = crearSemaforoPokemon(getPoke->nombre);
				indexSem = list_add(semaforosPokemon,semPoke);
			}
			
			t_getPokemon_indexSem* getPokeSem = crearGetPokemonIndexSem(indexSem, getPoke);
			pthread_mutex_unlock(&mutexListaSemsGet);

			pthread_t hiloAtenderGetPoke;
			pthread_create(&hiloAtenderGetPoke, NULL, (void*)atenderGetPokemon,(void*)getPokeSem);
			pthread_detach(hiloAtenderGetPoke);
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

void serve_suscribe(uint32_t* socket)
{
	int cod_op;
	t_buffer* buffer;
	int recvRes = recv(*socket, &cod_op, sizeof(int), MSG_WAITALL);
	if(recvRes == -1){
		cod_op = -1;
	}
	if(cod_op > 0 && cod_op < 11) {
		buffer = recibir_buffer(*socket);
		if(buffer->size > 1000 || buffer->size <= 0) {
			log_debug(logger,"Entro en el if");
			cod_op = -1;
			free(buffer->stream);
			free(buffer);
		}
	}
	process_suscribe_request(cod_op, buffer, *socket);
}

void process_suscribe_request(uint32_t cod_op, t_buffer* buffer, uint32_t cliente_fd) {
	switch (cod_op) {
		case NEW_POKEMON:
		{
			t_new_pokemon *newPoke = deserializar_newPokemon(buffer);
			log_info(logger, "NEW_POKEMON ingresado: %s", newPoke->nombre);

			t_acknowledgement* ack = crearAcknowledgement(idModule,newPoke->ID_mensaje_recibido, NEW_POKEMON);

			uint32_t indexSem = getIndexSemaforo(newPoke->nombre, semaforosPokemon);
			if(indexSem == ERROR){
			 	t_semaforo_pokemon* semPoke = crearSemaforoPokemon(newPoke->nombre);
				indexSem = list_add(semaforosPokemon,semPoke);
			}
			
			t_newPokemon_indexSem* newPokeSem = crearNewPokemonIndexSem(indexSem, newPoke);


			pthread_t sendAck;
			pthread_create(&sendAck, NULL, (void*)enviarAck, ack);
			pthread_detach(sendAck);

			pthread_t hiloAtenderNewPoke;
			pthread_create(&hiloAtenderNewPoke, NULL, atenderNewPokemon,(void*)newPokeSem);
			pthread_detach(hiloAtenderNewPoke);			

			free(buffer->stream);
			free(buffer);
			break;
		}
		case CATCH_POKEMON:
		{
			t_catch_pokemon *catchPoke = deserializar_catchPokemon(buffer);

			log_info(logger, "CATCH_POKEMON ingresado: %s", catchPoke->nombre);

			t_acknowledgement* ack = crearAcknowledgement(idModule ,catchPoke->ID_mensaje_recibido, CATCH_POKEMON);

			uint32_t indexSem = getIndexSemaforo(catchPoke->nombre, semaforosPokemon);

			if(indexSem == ERROR){
			 	t_semaforo_pokemon* semPoke = crearSemaforoPokemon(catchPoke->nombre);
				indexSem = list_add(semaforosPokemon,semPoke);
			}
			
			t_catchPokemon_indexSem* catchPokeSem = crearCatchPokemonIndexSem(indexSem, catchPoke);

			pthread_t sendAck;
			pthread_create(&sendAck, NULL, (void*)enviarAck, ack);
			pthread_detach(sendAck);

			pthread_t hiloAtenderCatchPoke;
			pthread_create(&hiloAtenderCatchPoke, NULL, (void*)atenderCatchPokemon,(void*)catchPokeSem);
			pthread_detach(hiloAtenderCatchPoke);


			free(buffer->stream);
			free(buffer);
		
			break;
		}
		case GET_POKEMON:
		{
			t_get_pokemon *getPoke = deserializar_getPokemon(buffer);
			
			log_info(logger, "GET_POKEMON ingresado: %s", getPoke->nombre);			

			t_acknowledgement* ack = crearAcknowledgement(idModule ,getPoke->ID_mensaje_recibido, GET_POKEMON);

			uint32_t indexSem = getIndexSemaforo(getPoke->nombre, semaforosPokemon);
			if(indexSem == ERROR){
			 	t_semaforo_pokemon* semPoke = crearSemaforoPokemon(getPoke->nombre);
				indexSem = list_add(semaforosPokemon,semPoke);
			}
			
			t_getPokemon_indexSem* getPokeSem = crearGetPokemonIndexSem(indexSem, getPoke);

			pthread_t sendAck;
			pthread_create(&sendAck, NULL, (void*)enviarAck, ack);
			pthread_detach(sendAck);

			pthread_t hiloAtenderGetPoke;
			pthread_create(&hiloAtenderGetPoke, NULL, (void*)atenderGetPokemon,(void*)getPokeSem);
			pthread_detach(hiloAtenderGetPoke);

			free(buffer->stream);
			free(buffer);
			
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
	if(size > 0 || size < 1000){
		buffer->size = size;
		buffer->stream = malloc(buffer->size);
		recv(socket_cliente, buffer->stream, buffer->size, MSG_WAITALL);
	}

	return buffer;
}

uint32_t escuchaBroker(){
	char* IP_BROKER = config_get_string_value(config,"IP_BROKER");
    char* PUERTO_BROKER = config_get_string_value(config,"PUERTO_BROKER");
	uint32_t conexion = crear_conexion(IP_BROKER, PUERTO_BROKER);

	return conexion;
}

void mandarAPPEARED(t_appeared_pokemon* appearedPoke) {
    
    t_paquete* paquete = serializar_appearedPokemon(appearedPoke);

    uint32_t conexion = crear_conexion(IP_BROKER, PUERTO_BROKER);

	if(conexion == ERROR){
		log_error(logger,"No se pudo mandar el Appeared de %s por Broker caido", appearedPoke->nombre);
	}else{
		enviarMensaje(paquete, conexion);
		log_debug(logger,"MENSAJE APPEARED ENVIADO");

		int cod_op;
		t_buffer* buffer;
		recv(conexion, &cod_op, sizeof(int), MSG_WAITALL);
		buffer = recibir_buffer(conexion);
		if(buffer->size > 0 && buffer->size < 1000){
			t_id_mensaje_recibido* confirmacion = deserializar_idMensajeRecibido(buffer);

			log_info(logger,"El id de mensaje APPEARED_POKEMON asignado es %i\n", (uint32_t)confirmacion->id_mensajeEnviado);
		}
		free(buffer->stream);
		free(buffer);
	}

	if(conexion != ERROR){
    	liberar_conexion(conexion);
	}
}

void mandarCAUGHT(t_caught_pokemon* caughtPoke) {
    
    t_paquete* paquete = serializar_caughtPokemon(caughtPoke);

    uint32_t conexion = crear_conexion(IP_BROKER, PUERTO_BROKER);

	if(conexion == ERROR){
		log_error(logger,"No se pudo mandar el Caught por Broker caido");
	}else{
		enviarMensaje(paquete, conexion);
		log_debug(logger,"MENSAJE CAUGHT ENVIADO");

		int cod_op;
		t_buffer* buffer;
		recv(conexion, &cod_op, sizeof(int), MSG_WAITALL);
		buffer = recibir_buffer(conexion);
		if(buffer->size > 0 && buffer->size < 1000){
			t_id_mensaje_recibido* confirmacion = deserializar_idMensajeRecibido(buffer);
			log_info(logger,"El id de mensaje CAUGHT_POKEMON asignado es %i\n", (uint32_t)confirmacion->id_mensajeEnviado);
		}
		free(buffer->stream);
		free(buffer);
	}

    if(conexion != ERROR){
    	liberar_conexion(conexion);
	}
}

void mandarLOCALIZED(t_localized_pokemon* localizedPoke) {
    
    t_paquete* paquete = serializar_localizedPokemon(localizedPoke);

    uint32_t conexion = crear_conexion(IP_BROKER, PUERTO_BROKER);

	if(conexion == ERROR){
		log_error(logger,"No se pudo mandar el Localized de %s por Broker caido", localizedPoke->nombre);
	}else{
		enviarMensaje(paquete, conexion);
		log_debug(logger,"MENSAJE LOCALIZED ENVIADO");

		int cod_op;
		t_buffer* buffer;
		recv(conexion, &cod_op, sizeof(int), MSG_WAITALL);
		buffer = recibir_buffer(conexion);
		if(buffer->size > 0 && buffer->size < 1000){
			t_id_mensaje_recibido* confirmacion = deserializar_idMensajeRecibido(buffer);

			log_info(logger,"El id de mensaje LOCALIZED_POKEMON asignado es %i\n", (uint32_t)confirmacion->id_mensajeEnviado);
		}
		free(buffer->stream);
		free(buffer);
	}

    if(conexion != ERROR){
    	liberar_conexion(conexion);
	}
}


