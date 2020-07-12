#ifndef TEAM_H_
#define TEAM_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<pthread.h>
#include<semaphore.h>
#include<readline/readline.h>
#include<commons/collections/list.h>

#include "sockets.h"
#include "utils.h"
#include "models.h"
#include "planificador.h"

char* IP;
char* PUERTO;
char* LOG;
char* ALGORITMO;

//GLOBALES
t_list* objetivoGlobal;
t_list* pokemonesEnMapa;

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
sem_t mutexPokesEnMapa;

sem_t counterPokesEnMapa;

uint32_t cicloCPU;
uint32_t quantum;
uint32_t currentQuantum;

//INICIALIZACION Y TERMINACION DEL MODULO
void inicializarTeam();
void terminar_programa(int, t_log*, t_config*);

#endif /* TEAM_H_ */