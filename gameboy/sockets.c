#include "sockets.h"
#include "gameboy.h"

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
	if(send(socket_cliente,stream,sizePaquete,MSG_CONFIRM) == -1)
	{
		log_error(logger, "Send error");
		//Desuscribir a ese gil
		liberar_conexion(socket_cliente);
		exit(1);
	}
	liberarPaquete(paquete);
	free(stream);
}

void modoSuscriptor(char* arg) {

    uint32_t conexion = crear_conexion(ipBroker, puertoBroker);
	uint32_t mq = getColaDeMensajes(arg);
	t_register_module* registerModule = crearSuscribe(mq, ID_MODULE); //TODO: Tomar del config
	t_paquete *paquete = serializar_registerModule(registerModule);

	free(registerModule);
    enviarMensaje(paquete, conexion);
	while(1) 
	{
		serve_client(&conexion);
	}

	liberar_conexion(conexion);
	log_info(logger, "Conexion con broker finalizada");
}


void temporizador (void* tiempo) {
	uint32_t temp = (uint32_t) tiempo;
	sleep(temp);
	//exit(0);
	pthread_exit(&hiloSuscriptor);
}

void serve_client(uint32_t* socket)
{
	uint32_t cod_op;
	if(recv(*socket, &cod_op, sizeof(int), 0) == -1){
		cod_op = -1;
	}
	t_buffer *buffer = recibir_buffer(*socket);
	process_request(buffer, cod_op, *socket);
}

void enviarAck(t_acknowledgement* ack) {
	t_paquete* paquete = serializar_acknowledgement(ack);
	free(ack);
	uint32_t respuestaBroker = crear_conexion(ipBroker, puertoBroker);
	enviarMensaje(paquete, respuestaBroker);
	liberar_conexion(respuestaBroker);
	return;
}

void process_request(t_buffer *buffer, uint32_t operation_cod, uint32_t socket_cliente) {
	switch (operation_cod)
	{
		case NEW_POKEMON:
		{
			t_new_pokemon* newPoke = deserializar_newPokemon(buffer);
	
			log_info(logger, "Recibido new_pokemon de nombre %s", newPoke->nombre);

			t_acknowledgement* ack = crearAcknowledgement(ID_MODULE, newPoke->ID_mensaje_recibido, NEW_POKEMON);
			
			pthread_t sendAck;
			pthread_create(&sendAck, NULL, (void*)enviarAck, ack);
			pthread_detach(sendAck);

			break;
		}
		case APPEARED_POKEMON:
		{
			t_appeared_pokemon* appearedPoke = deserializar_appearedPokemon(buffer);
	
			log_info(logger, "Se ha recibido un mensaje del BROKER por la cola APPEARED_POKEMON");
			log_info(logger, "Recibido appeared_pokemon de nombre %s", appearedPoke->nombre);

			break;
		}
		case GET_POKEMON:
		{
			t_get_pokemon* getPoke = deserializar_getPokemon(buffer);
		
			log_info(logger, "Se ha recibido un mensaje del BROKER por la cola GET_POKEMON");
			log_info(logger, "Recibido get_pokemon de nombre %s", getPoke->nombre);

			break;
		}
		case CAUGHT_POKEMON:
		{
			t_caught_pokemon* caughtPoke = deserializar_caughtPokemon(buffer);

			log_info(logger, "Se ha recibido un mensaje del BROKER por la cola CAUGHT_POKEMON");
			log_info(logger, "Recibido caught_pokemon ID: %i e ID_MensajeCorrelativo %i", caughtPoke->ID_mensaje_recibido, caughtPoke->ID_mensaje_original);

			break;
		}
		case LOCALIZED_POKEMON:
		{
			t_localized_pokemon* localizedPoke = deserializar_localizedPokemon(buffer);
	
			log_info(logger, "Se ha recibido un mensaje del BROKER por la cola LOCALIZED_POKEMON");
			log_info(logger, "Recibido localized_pokemon de nombre %s", localizedPoke->nombre);

			break;
		}
		case CATCH_POKEMON:
		{
			t_catch_pokemon* catchPoke = deserializar_catchPokemon(buffer);

			log_info(logger, "Se ha recibido un mensaje del BROKER por la cola CATCH_POKEMON");
			log_info(logger, "Recibido catch_pokemon de nombre %s", catchPoke->nombre);

			break;
		}
		case MENSAJE_RECIBIDO:
		{
			t_id_mensaje_recibido* idMensajeEnviado = deserializar_idMensajeRecibido(buffer);

			log_info(logger, "Se ha recibido el id del mensaje enviado: %i", idMensajeEnviado->id_mensajeEnviado);
			liberar_conexion(socket_cliente);
			break;
		}
		case SUBSCRIBE:
		{
			t_suscripcion* suscripcion = deserializar_suscripcion(buffer);
			log_info(logger, "Id asignado en la suscripcion: %i", suscripcion->idModule);
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