#include "sockets.h"

/*CLIENT SIDE*/

int crear_conexion(char *ip, char* puerto) {
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	uint32_t socket_cliente = socket(server_info->ai_family,
			server_info->ai_socktype, server_info->ai_protocol);

	if (connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen)
			== -1)
		printf("error");

	freeaddrinfo(server_info);

	return socket_cliente;
}

void liberar_conexion(uint32_t socket_cliente) {
	close(socket_cliente);
}

void enviarMensaje(t_paquete* paquete, uint32_t socket_cliente) {
	uint32_t sizePaquete = paquete->buffer->size + 2 * sizeof(uint32_t);
	void* stream = serializar_paquete(paquete, sizePaquete);
	send(socket_cliente,stream,sizePaquete,0);
	liberarPaquete(paquete);
	free(stream);
}

void mandarGET() {

	char* ip = config_get_string_value(config, "IP_BROKER");
    char* puerto = config_get_string_value(config, "PUERTO_BROKER");
    uint32_t conexion = crear_conexion(ip, puerto);

	for(uint32_t i=0;i<list_size(objetivoGlobal);i++) {
		t_pokemon_cantidad* poke = (t_pokemon_cantidad*)list_get(objetivoGlobal,i);
		if(poke->cantidad > 0) {
			//sendGET(poke->nombre);
			t_get_pokemon* getPokemon = crearGetPokemon(11, poke->nombre);
			t_paquete* paquete = serializar_getPokemon(getPokemon);
			printf("GET %s\n",poke->nombre);
			free(getPokemon);
			enviarMensaje(paquete, conexion);
		}
	}
	liberar_conexion(conexion);
}

void suscribe(void* structSuscribe) {
	t_suscribe* s = (t_suscribe*) structSuscribe;

	t_register_module* suscribe = crearSuscribe(s->messageQueue);
	t_paquete* paquete = serializar_registerModule(suscribe);
	printf("SUSCRIBE %i \n", s->messageQueue);
	free(suscribe);
	enviarMensaje(paquete, s->conexion);

	while(1) {
		serve_client(&s->conexion);
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

    for (p=servinfo; p != NULL; p = p->ai_next)
    {
        if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
            continue;

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_servidor);
            continue;
        }
        break;
    }

	listen(socket_servidor, SOMAXCONN);

    freeaddrinfo(servinfo);

	printf("Iniciando servidor...\n");
    while(1)
    	esperar_cliente(socket_servidor);
}

void esperar_cliente(uint32_t socket_servidor)
{
	struct sockaddr_in dir_cliente;

	uint32_t tam_direccion = sizeof(struct sockaddr_in);

	uint32_t socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

	pthread_create(&thread,NULL,(void*)serve_client,&socket_cliente);
	pthread_join(thread, NULL);

}

void serve_client(uint32_t* socket)
{
	
	uint32_t cod_op;
	if(recv(*socket, &cod_op, sizeof(int), MSG_WAITALL) == -1)
		cod_op = -1;
	//printf("codigo de op: %i", cod_op);
	process_request(cod_op, *socket);
}

void process_request(uint32_t cod_op, uint32_t cliente_fd) {
	t_buffer* buffer = recibir_buffer(cliente_fd);
	switch (cod_op) {
		case APPEARED_POKEMON:
			{
				//log_info(logger, "SIZE BUFFER EN NEW: %i", buffer->size);
				// deserializar_appearedPokemon(buffer);
				t_appeared_pokemon* appearedPoke = deserializar_appearedPokemon(buffer);
				t_pokemon_posicion* poke = crearPokemonPosicion(appearedPoke->nombre, appearedPoke->posicion);
				insertPokeEnMapa(poke);

				free(appearedPoke);
				free(buffer->stream);
				free(buffer);
				break;
			}
		case CAUGHT_POKEMON:
			{
				t_caught_pokemon* caughtPoke = deserializar_caughtPokemon(buffer);
				printf("\nEl pokemon del mensaje '%i'", caughtPoke->ID_mensaje_original);
				if(caughtPoke->catchStatus == 1) {
					printf(" ha sido capturado\n");
				} else {
					printf(" no se pudo capturar\n");
				}

				free(caughtPoke);
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

t_buffer* recibir_buffer(uint32_t socket_cliente)
{
	t_buffer* buffer = malloc(sizeof(t_buffer));
	int size;
    
    recv(socket_cliente, &size, sizeof(int), MSG_WAITALL);
	printf("size buffer: %d\n", size);
    buffer->size = size;
	buffer->stream = malloc(buffer->size);
	recv(socket_cliente, buffer->stream, buffer->size, MSG_WAITALL);

	return buffer;
}

uint32_t escuchaBroker(){
	char* IP_BROKER = config_get_string_value(config,"IP_BROKER");
    char* PUERTO_BROKER = config_get_string_value(config,"PUERTO_BROKER");

	return crear_conexion(IP_BROKER, PUERTO_BROKER);
}

