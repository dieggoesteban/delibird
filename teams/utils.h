
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

//GLOBAL
t_log* logger;
t_config* config;
t_list* objetivoGlobal;

//INICIALIZACION
u_int32_t getCantidadEntrenadores();
void inicializarEntrenadores();
void setObjetivoGlobal();

//CREAR
t_pokemon_posicion* crearPokemonPosicion(char* nombre, t_posicion* posicion);
t_entrenador* crearEntrenador(t_posicion* posicion, t_list* objetivos, t_list* pokemon, uint32_t cantidadObjetivo);
t_pokemon_cantidad* setPokemonCantidad(char* nombre, uint32_t cantidad);

//COLAS
t_list* colaNEW;
t_list* colaREADY;
t_list* colaEXEC;
t_list* colaBLOCKED;
t_list* colaEXIT;

//UTILIDADES ARRAYS/LISTS
uint32_t arraySize(void* arr[]);
t_list* arrayToList(void* arr[]);
uint32_t perteneceAlArray(char* val, char* arr[], uint32_t size);
uint32_t perteneceALista(char *val, t_list* lista);
uint32_t perteneceAListaContador(char *val, t_list* lista);

//STRUCTS PERTENECEN A LISTA
uint32_t pokemonCantidadPerteneceALista(t_pokemon_cantidad* pokemon, t_list* lista);
uint32_t pokemonPosicionPerteneceALista(t_pokemon_posicion* pokemon, t_list* lista);
uint32_t entrenadorPerteneceALista(t_entrenador* entrenador, t_list* lista);

//OBTENER DE LISTA
t_list* obtenerEntrenadoresSinDeadlock(); //obtiene de colaBlocked


#endif /* UTILS_H_ */
