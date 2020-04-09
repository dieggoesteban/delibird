#ifndef CONNECTION_H_
#define CONNECTION_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include "utils.h"

typedef enum
{
	NEW_POKEMON = 1,
	APPEARED_POKEMON = 2,
	CATCH_POKEMON = 3,
	CAUGHT_POKEMON = 4,
	GET_POKEMON = 5,
	LOCALIZED_POKEMON = 6
} mq_cod;

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	mq_cod codigo_mensaje;
	t_buffer* buffer;
} t_paquete;

typedef struct
{
	uint32_t posicion_x;
	uint32_t posicion_y;
	uint32_t cantidad;
} t_posicion_cantidad;

typedef struct
{
	uint32_t posicion_x;
	uint32_t posicion_y;
} t_posicion;

typedef struct
{
	uint32_t ID_mensaje_recibido;
	uint32_t sizeNombre;
	char* nombre;
	t_posicion_cantidad* posicionCantidad;
} t_new_pokemon;

typedef struct{
	uint32_t ID_mensaje_recibido_originalmente;
	uint32_t sizePokemon;
	char* nombre;
	uint32_t sizePosicion_cantidad; //LO AGREGO POR AHORA
	t_list* posicion_cantidad; //una lista de t_posicion_cantidad
}t_localized_pokemon;

typedef struct{
	uint32_t ID_mensaje_recibido;
	uint32_t sizeNombre;
	char* nombre;
	t_posicion* posicion;
} t_appeared_pokemon;

typedef struct{
	uint32_t ID_mensaje_recibido;
	uint32_t sizeNombre;
	char* nombre;
	t_posicion* posicion;
}t_catch_pokemon;

typedef struct{
	uint32_t ID_mensaje_recibido;
	uint32_t catchStatus; 
}t_caught_pokemon;

typedef struct{
	uint32_t ID_mensaje_recibido;
	uint32_t sizeNombre;
	char* nombre;
} t_get_pokemon;


int crear_conexion(char* ip, char* puerto);
void enviar_mensaje(char* mensaje, int socket_cliente);
char* recibir_mensaje(int socket_cliente);
t_new_pokemon* deserializar_newPokemon(t_buffer* buffer);
t_paquete* serializar_newPokemon(t_new_pokemon* newPokemon);
t_paquete* serializar_appearedPokemon(t_appeared_pokemon* pokemon);
t_appeared_pokemon* deserializar_appearedPokemon(t_buffer* buffer);
t_paquete* serializar_catchPokemon(t_catch_pokemon* pokemon);
t_catch_pokemon* deserializar_catchPokemon(t_buffer* buffer);
t_paquete* serializar_caughtPokemon(t_caught_pokemon* pokemon);
t_catch_pokemon* deserializar_caughtPokemon(t_buffer* buffer);
t_paquete* serializar_getPokemon(t_get_pokemon* pokemon);
t_catch_pokemon* deserializar_getPokemon(t_buffer* buffer);
t_paquete* serializar_localizedPokemon(t_localized_pokemon* localizedPokemon);
t_localized_pokemon* deserializar_localizedPokemon(t_buffer* buffer);

// void eliminar_paquete(t_paquete* paquete);
// void liberar_conexion(int socket_cliente);

#endif /* CONNECTION_H_ */


		// paquete = serializar_newPokemon(newPokemon);
		
		// t_new_pokemon* pikachu = deserializar_newPokemon(paquete->buffer);
		// printf("nombre del poke: %s", pikachu->nombre);
		

		// free(paquete->buffer->stream);
		// free(paquete->buffer);
		// free(paquete);