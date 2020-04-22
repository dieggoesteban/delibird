
#ifndef UTILS_H_
#define UTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<commons/string.h>
#include<commons/config.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>

#include "serializador.h"

t_log* logger;
t_config* config;


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

uint32_t perteneceAlArray(char* val, char* arr[], uint32_t size);
t_entrenador* crearEntrenador(t_posicion* posicion, t_list* objetivos, t_list* pokemon);
t_list* inicializarEntrenadores();

#endif /* UTILS_H_ */
