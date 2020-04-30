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

#include "models.h"
#include "utils.h"
// #include "serializador.h"

t_list* colaNEW;
t_list* colaREADY;
t_list* colaEXEC;
t_list* colaBLOCKED;
t_list* colaEXIT;

t_list* pokemonesEnMapa;
t_list* objetivoGlobal;

//INICIALIZACION HILOS

t_entrenador* getEntrenadorMejorPosicionado(t_pokemon_posicion* pokemon);
uint32_t moverEntrenadorDeCola(t_list* colaReceptora, t_list* colaEmisora, t_entrenador* entrenador);
void asignarPokemonAEntrenador(t_pokemon_posicion* pokemon);

#endif /* PLANIFICADOR_H_ */