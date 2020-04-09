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
	uint32_t ID_mensaje_recibido;
	uint32_t sizeNombre;
	char* nombre;
	t_posicion_cantidad* posicionCantidad;
} t_new_pokemon;

typedef struct{
	uint32_t ID_mensaje_recibido_originalmente;
	uint32_t sizePokemon;
	char* pokemon;
	t_list* posicion_cantidad; //una lista de t_posicion_cantidad
}t_localized_pokemon;


int crear_conexion(char* ip, char* puerto);
void enviar_mensaje(char* mensaje, int socket_cliente);
char* recibir_mensaje(int socket_cliente);
t_new_pokemon* deserializar_newPokemon(t_buffer* buffer);
t_paquete* serializar_newPokemon(t_new_pokemon* newPokemon);
// void eliminar_paquete(t_paquete* paquete);
// void liberar_conexion(int socket_cliente);

#endif /* CONNECTION_H_ */