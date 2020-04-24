
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

#include "planificador.h"

t_log* logger;
t_config* config;

uint32_t perteneceAlArray(char* val, char* arr[], uint32_t size);
uint32_t perteneceALista(char *val, t_list* lista);
uint32_t perteneceAListaContador(char *val, t_list* lista);
t_entrenador* crearEntrenador(t_posicion* posicion, t_list* objetivos, t_list* pokemon, uint32_t cantidadObjetivo);
t_list* inicializarEntrenadores();
void setObjetivoGlobal(t_list* entrenadores);
u_int32_t getCantidadEntrenadores();
t_pokemon_cantidad* setPokemonCantidad(char* nombre, uint32_t cantidad);


#endif /* UTILS_H_ */