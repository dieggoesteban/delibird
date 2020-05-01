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

t_list* pokemonesEnMapa; // lucario pikachu pikachu pikachu squirtle
t_list* objetivoGlobal; //3 lucario 1 pikachu 2 squirtle

//INICIALIZACION HILOS

t_entrenador *getEntrenadorMejorPosicionado(t_pokemon_posicion *pokemon, t_list* entrenadores);
uint32_t moverEntrenadorDeCola(t_list* colaReceptora, t_list* colaEmisora, t_entrenador* entrenador);
bool asignarPokemonAEntrenador();
bool pokemonEnObjetivoGlobal(t_pokemon_posicion* pokemon);

#endif /* PLANIFICADOR_H_ */