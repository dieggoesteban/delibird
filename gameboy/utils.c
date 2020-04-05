
#include "utils.h"

//TODO
/*
 * Recibe un paquete a serializar, y un puntero a un int en el que dejar
 * el tamaño del stream de bytes serializados que devuelve
 */
// void* serializar_paquete(t_paquete* paquete, int bytes)
// {
// 	void * magic = malloc(bytes);
// 	int desplazamiento = 0;

// 	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
// 	desplazamiento+= sizeof(int);
// 	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
// 	desplazamiento+= sizeof(int);
// 	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
// 	desplazamiento+= paquete->buffer->size;

// 	return magic;
// }

// int crear_conexion(char *ip, char* puerto)
// {
// 	struct addrinfo hints;
// 	struct addrinfo *server_info;

// 	memset(&hints, 0, sizeof(hints));
// 	hints.ai_family = AF_UNSPEC;
// 	hints.ai_socktype = SOCK_STREAM;
// 	hints.ai_flags = AI_PASSIVE;

// 	getaddrinfo(ip, puerto, &hints, &server_info);

// 	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

// 	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1)
// 		printf("error");

// 	freeaddrinfo(server_info);

// 	return socket_cliente;
// }

// //TODO
// void enviar_mensaje(char* mensaje, int socket_cliente)
// {
// 	t_paquete* paquete = malloc(sizeof(t_paquete));

// 	paquete->codigo_operacion = MENSAJE;
// 	paquete->buffer = malloc(sizeof(t_buffer));
// 	paquete->buffer->size = strlen(mensaje)+1;
// 	paquete->buffer->stream = malloc(paquete->buffer->size);
// 	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

// 	int sizePaquete = paquete->buffer->size + 2 * sizeof(int);

// 	void* a_enviar = serializar_paquete(paquete,sizePaquete);

// 	send(socket_cliente,a_enviar,sizePaquete,0);

// 	free(a_enviar);
// 	free(paquete->buffer->stream);
// 	free(paquete->buffer);
// 	free(paquete);

// }

// //TODO
// char* recibir_mensaje(int socket_cliente)
// {
// 	int cod_op, size;
// 	char* mensaje;
// 	recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL);

// 	if(cod_op == MENSAJE) {
// 		recv(socket_cliente, &size, sizeof(int), MSG_WAITALL);
// 		mensaje = malloc(size);
// 		recv(socket_cliente, mensaje, size, MSG_WAITALL);

// 		return mensaje;
// 	} else {
// 		return "No recibi nada";
// 	}
// }

// void liberar_conexion(int socket_cliente)
// {
// 	close(socket_cliente);
// }

void procesarComando(int argc, char* argv[]) {
	
	char* proceso = argv[1];
	//char* tipo_mensaje = argv[1];
	char* ip;
	char* puerto;

	if(strcmp(proceso,"BROKER") == 0 || strcmp(proceso,"SUSCRIPTOR") == 0) {
		ip = config_get_string_value(config,"IP_BROKER");
		puerto = config_get_string_value(config,"PUERTO_BROKER");
	}
	else if(strcmp(proceso,"TEAM") == 0) {
		ip = config_get_string_value(config,"IP_TEAM");
		puerto = config_get_string_value(config,"PUERTO_TEAM");
	}
	else if(strcmp(proceso,"GAMECARD") == 0) {
		ip = config_get_string_value(config,"IP_GAMECARD");
		puerto = config_get_string_value(config,"PUERTO_GAMECARD");
	}

	log_info(logger,ip);
	log_info(logger,puerto);

	// if(strcmp(tipo_mensaje,"NEW_POKEMON") == 0) {
		
	// }
	// else if(strcmp(tipo_mensaje,"APPEARED_POKEMON") == 0) {
		
	// }
	// else if(strcmp(tipo_mensaje,"CATCH_POKEMON") == 0) {
		
	// }
	// else if(strcmp(tipo_mensaje,"CAUGHT_POKEMON") == 0) {
		
	// }
	// else if(strcmp(tipo_mensaje,"GET_POKEMON") == 0) {
		
	// }
	// else if(strcmp(tipo_mensaje,"CATCH_POKEMON") == 0) {
		
	// }
	// else if(strcmp(tipo_mensaje,"CAUGHT_POKEMON") == 0) {
		
	// }
	// else if(strcmp(tipo_mensaje,"GET_POKEMON") == 0) {
		
	// }
	// else if(strcmp(tipo_mensaje,"LOCALIZED_POKEMON") == 0) {
		
	// }
}