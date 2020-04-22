#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include <commons/collections/list.h>

#include "serializador.h"

uint32_t pid;

typedef struct
{
    char* nombre;
    t_posicion* posicion;
} t_pokemon;

typedef struct
{
	uint32_t id;
	t_posicion* posicion;
    t_list* pokemonCapturados;
    t_list* pokemonObjetivo;
    t_pokemon* pokemonPlanificado;
    int deadlock; // 0 y 1 por ahora, es solo un mockup
} t_entrenador;

void inicializarPid();
uint32_t getNuevoPid();

//METODOS DE CONEXION
// int crear_conexion(char* ip, char* puerto);
// void enviarMensaje(t_paquete* paquete, uint32_t socket_cliente);
// char* recibir_mensaje(int socket_cliente);
// void liberar_conexion(int socket_cliente);

#endif /* PLANIFICADOR_H_ */