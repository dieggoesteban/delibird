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
#include <commons/collections/queue.h>

#include "models.h"
#include "utils.h"
#include "algoritmos.h"
#include "sockets.h"

typedef t_entrenador* (AlgoritmoFunc)(t_entrenador*);

//GLOBALES
t_list* colaNEW;
t_list* colaREADY;
t_list* colaEXEC;
t_list* colaBLOCKED;
t_list* colaEXIT;

sem_t mutexNEW;
sem_t mutexREADY;
sem_t mutexEXEC;
sem_t mutexBLOCKED;
sem_t mutexEXIT;
sem_t counterPokesEnMapa;
sem_t counterEntrenadoresCatch;
sem_t mutexEntrenadoresCatch;
sem_t pokesObjetivoGlobal;
sem_t mutexDetector;
sem_t estaDesconectado;

t_list* objetivoGlobal;
t_list* pokemonesEnMapa; // lucario pikachu pikachu pikachu squirtle
t_list* objetivoGlobal; //3 lucario 1 pikachu 2 squirtle
t_list* entrenadoresCatch;

uint32_t cicloCPU;
bool desalojo;

//INICIALIZACION HILOS

t_entrenador *getEntrenadorMejorPosicionado(t_pokemon_posicion *pokemon, t_list* entrenadores);
bool moverEntrenadorDeCola(t_list* colaReceptora, t_list* colaEmisora, t_entrenador* entrenador);
bool asignarPokemonAEntrenador();
t_entrenador* asignarAEntrenador(t_pokemon_posicion* pokemon);
bool pokemonEnObjetivoGlobal(t_pokemon_posicion* pokemon);
void planificarFIFO();
void ejecutarEntrenador(t_entrenador* entrenador);
void mandarCATCH(t_entrenador* entrenador);
t_entrenador_posicion* getIntercambio(t_entrenador* tr1, t_entrenador* tr2);

void* planificadorREADY();
void* planificadorEXEC(void*);
void* detectorDeIntercambio();
void* modoDesconectado();

#endif /* PLANIFICADOR_H_ */