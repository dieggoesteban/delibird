
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
#include<commons/collections/queue.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>

#include "models.h"
#include "serializador.h"
#include "sockets.h"
#include "planificador.h"

//GLOBAL
t_log* logger;
t_config* config;
t_list* objetivoGlobal;
t_list* pokemonesEnMapa;
uint32_t pid;
uint32_t cicloCPU;
uint32_t cantEntrenadores;

//INICIALIZACION
u_int32_t getCantidadEntrenadores();
void inicializarPid();
uint32_t getNuevoPid();
void inicializarEntrenadores();
void setObjetivoGlobal();
void actualizarObjetivoGlobal(char* poke, bool restar);

//SEMAFOROS

sem_t mutexEXEC;
sem_t mutexEXIT;
sem_t mutexBLOCKED;
sem_t mutexPokesEnMapa;
sem_t mutexDetector;
sem_t counterPokesEnMapa;
sem_t mutexEntrenadoresCatch;
sem_t counterEntrenadoresCatch;

//CREAR
t_pokemon_posicion* crearPokemonPosicion(char* nombre, t_posicion* posicion);
t_entrenador_posicion* crearEntrenadorPosicion(uint32_t id, t_posicion* posicion, t_entrenador* tr);

t_entrenador* crearEntrenador(t_posicion* posicion, t_list* objetivos, t_list* pokemon, uint32_t cantidadObjetivo);
t_pokemon_cantidad* setPokemonCantidad(char* nombre, uint32_t cantidad);
t_entrenador_catch* crearEntrenadorCatch(uint32_t mID, uint32_t trID);
t_suscribe* getSuscribe(uint32_t mq);

//COLAS
t_list* colaNEW;
t_list* colaREADY;
t_list* colaEXEC;
t_list* colaBLOCKED;
t_list* colaEXIT;
t_list* entrenadoresCatch;
t_list* trIds;
t_list* listaDeadlocks;

//UTILIDADES ARRAYS/LISTS/QUEUE
uint32_t arraySize(void* arr[]);
t_list* arrayToList(void* arr[]);
bool perteneceAlArray(char* val, char* arr[], uint32_t size);
bool perteneceALista(char *val, t_list* lista);
uint32_t getIndexList(char *val, t_list* lista);
uint32_t perteneceAListaContador(char *val, t_list* lista);
bool list_equals(t_list* list1, t_list* list2);
t_queue* listToQueue(t_list* lista);

//STRUCTS PERTENECEN A LISTA
uint32_t pokemonCantidadPerteneceALista(t_pokemon_cantidad* pokemon, t_list* lista);
uint32_t pokemonPosicionPerteneceALista(char* pokemon, t_list* lista);
uint32_t entrenadorPerteneceALista(t_entrenador* entrenador, t_list* lista);

//OBTENER DE LISTA
t_list* obtenerEntrenadoresSinDeadlock(); //obtiene de colaBlocked
uint32_t getIndexSemaforo(char* nombrePoke,t_list* lista);
t_list* pokesQueNoQuiere(t_entrenador* tr);
t_list* pokesQueSiQuiere(t_entrenador* tr, t_list* pokes);
uint32_t getEntrenadorByID(uint32_t id, t_list* lista);

//DE POKEMON
bool pokemonEnObjetivoGlobal(t_pokemon_posicion* pokemon);
void insertPokeEnMapa(t_pokemon_posicion* poke);

void procesarMensajeCaught(t_caught_pokemon* caughtPoke);

//DE ENTRENADOR
bool entrenadorEnDeadlock(t_entrenador* entrenador);
bool entrenadorCumplioObjetivo(t_entrenador* entrenador);
bool entrenadorDisponible(void* entrenador);
bool entrenadorLlegoASuDestino(void* entrenador);
bool tardaMenos(void* trA, void* trB);
bool ordenarEntrenador(void* a, void* b);
bool ordenarPokemon(void* a, void* b);
uint32_t turnosHastaPokemon(t_pokemon_posicion* pokemon, t_entrenador* entrenador);
uint32_t turnosHastaEntrenador(t_entrenador_posicion* tr, t_entrenador* entrenador);
t_entrenador* cambiarPosicionEntrenador(t_entrenador* entrenador, uint32_t posX, uint32_t posY);
void moverEntrenador(t_entrenador* entrenador);
void pasosParaIntercambio(t_entrenador* entrenador);
void pasosParaCaptura(t_entrenador* entrenador);
void actualizarPosicion(t_entrenador* entrenador);
void defaultCaptura(uint32_t index);
uint32_t getEstimacion(uint32_t estReal, uint32_t estAnterior);
uint32_t getTiempoReal(t_entrenador* tr);

//DE REPORTE DEADLOCK
t_deadlock* crearReporteDeadlock();
uint32_t perteneceReporteDeadlock(t_entrenador* tr);
t_deadlock* getReporteDeadlock(t_entrenador* tr);
t_trDeadlock* crearTrDeadlock(uint32_t idTr, bool deadlock);
bool sigueEnDeadlock(t_deadlock* dl);
t_trDeadlock* actualizarTrDeadlock(t_entrenador* tr, t_deadlock* dl);
#endif /* UTILS_H_ */
