#include "connection.h"

void* serializar_paquete(t_paquete* paquete, int bytes) {
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_mensaje), sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream,
			paquete->buffer->size);
	desplazamiento += paquete->buffer->size;

	return magic;
}

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


t_buffer* crear_buffer(uint32_t sizeStream, void* stream){
	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeStream;
	buffer->stream = malloc(sizeof(buffer->size));
	buffer->stream = stream;
	return buffer;
}

t_paquete* crear_paquete(uint32_t mq_cod, uint32_t sizeStream, void* stream){
	t_paquete* paquete = malloc(sizeof(t_paquete));
	// crear_buffer(sizeStream, stream);
	paquete->codigo_mensaje = mq_cod;
	paquete->buffer = crear_buffer(sizeStream, stream);

	return paquete;

}



t_paquete* serializar_newPokemon(t_new_pokemon* newPokemon){
	
	t_paquete* paquete = malloc(sizeof(t_paquete));
    t_buffer* newPokemon_buffer = malloc(sizeof(t_buffer));
    newPokemon_buffer->size = sizeof(uint32_t)*2 //ID_mensaje_recibido + sizePokemon
                            + strlen(newPokemon->nombre) + 1
                            + sizeof(t_posicion_cantidad);

    void* stream = malloc(newPokemon_buffer->size);
    int offset = 0;

    memcpy(stream + offset, &newPokemon->ID_mensaje_recibido, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &newPokemon->sizeNombre, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &newPokemon->nombre, strlen(newPokemon->nombre)+1);
    offset += strlen(newPokemon->nombre) +1;
    memcpy(stream + offset, &newPokemon->posicionCantidad->posicion_x, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &newPokemon->posicionCantidad->posicion_y, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &newPokemon->posicionCantidad->cantidad, sizeof(uint32_t));

	

    newPokemon_buffer->stream = stream;

	paquete = crear_paquete(NEW_POKEMON, newPokemon_buffer->size, newPokemon_buffer->stream);

	printf("codigo de mensaje en serialziar poke: %i", paquete->codigo_mensaje);
    //no se si se le deberia hacer un free o algo al struct de posicionCAntidad, pero no creo
    return paquete;

}


// t_paquete* serializar_mensaje(char* argumentos[], uint32_t MQ, uint32_t size_buffer){
// 	t_paquete* paquete = malloc(sizeof(t_paquete));
// 	void* stream = malloc(size_buffer);

// 	memcpy(stream, &argumentos, size_buffer);

// 	paquete = crear_paquete(MQ, size_buffer, stream);


// 	return paquete;

// }

t_new_pokemon* deserializar_newPokemon(t_buffer* buffer){
	t_new_pokemon* newPokemon = malloc(sizeof(t_new_pokemon));
	newPokemon->posicionCantidad = malloc(sizeof(t_posicion_cantidad));
	
	 void* stream = buffer->stream;

	 memcpy(&(newPokemon->ID_mensaje_recibido), stream, sizeof(uint32_t));
	 stream += sizeof(uint32_t);
	 memcpy(&(newPokemon->sizeNombre), stream, sizeof(uint32_t));
	 stream += sizeof(uint32_t);
	 memcpy(&(newPokemon->nombre), stream, sizeof(newPokemon->sizeNombre));
	 stream += sizeof(newPokemon->sizeNombre);
	 memcpy(&(newPokemon->posicionCantidad->posicion_x), stream, sizeof(uint32_t));
	 stream += sizeof(uint32_t);
	 memcpy(&(newPokemon->posicionCantidad->posicion_y), stream, sizeof(uint32_t));
	 stream += sizeof(uint32_t);
	//  memcpy(&(newPokemon->nombre), stream, sizeof(uint32_t));

	 memcpy(&(newPokemon->posicionCantidad->cantidad), stream, sizeof(uint32_t));
	 
	return newPokemon;

	

	//     t_persona* persona = malloc(sizeof(t_persona));
    
    // void* stream = buffer->stream;
    // Deserializamos los campos que tenemos en el buffer
    // memcpy(&(persona->dni), stream, sizeof(uint32_t));
    // stream += sizeof(uint32_t);
    // memcpy(&(persona->edad), stream, sizeof(uint8_t));
    // stream += sizeof(uint8_t);
    // memcpy(&(persona->pasaporte), stream, sizeof(uint32_t));
    // stream += sizeof(uint32_t);
    
    // Por último, para obtener el nombre, primero recibimos el tamaño y luego el texto en sí:
    // memcpy(&(persona->nombre_length), stream, sizeof(uint32_t));
    // stream += sizeof(uint32_t);
	//memcpy(&(persona->nombre), stream, sizeof(persona->nombre_length));
}





// char* recibir_mensaje(int socket_cliente) {
// 	t_paquete* paquete = malloc(sizeof(paquete));
// 	void* mensaje;
// 	int cod_op;
// 	int buffer_size;
// 	void* buffer;


// 	recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL);

// 	// recv(socket_cliente, &(paquete->codigo_operacion), sizeof(uint8_t),0);
// 	recv(socket_cliente, &buffer_size, sizeof(int), 0);
	

// 	printf("codigo de op: %d", cod_op);

// 	switch(cod_op){
// 	case MENSAJE:

// 		mensaje = malloc(buffer_size); 

// 		recv(socket_cliente, mensaje, buffer_size, 0);
// 		// printf("recibi esto %s", (char*)mensaje);
// 		return mensaje;
// 		break;
// 	}
// 	return "este es el return";
// }

void liberar_conexion(int socket_cliente) {
	close(socket_cliente);
}
