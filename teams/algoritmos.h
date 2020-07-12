#ifndef ALGORITMOS_H_
#define ALGORITMOS_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<pthread.h>
#include<semaphore.h>
#include<readline/readline.h>
#include<commons/collections/list.h>

#include "utils.h"

typedef t_entrenador* (AlgoritmoFunc)(t_entrenador*);

char* IP;
char* PUERTO;
char* LOG;

uint32_t quantum;
uint32_t currentQuantum;

//GLOBALES
t_list* objetivoGlobal;
t_list* pokemonesEnMapa;

t_list* colaNEW;
t_list* colaREADY;
t_list* colaEXEC;
t_list* colaBLOCKED;
t_list* colaEXIT;

pthread_t threadEXEC;

sem_t mutexNEW;
sem_t mutexREADY;
sem_t mutexEXEC;
sem_t mutexBLOCKED;
sem_t mutexEXIT;

t_entrenador* FIFO(t_entrenador* e);
t_entrenador* SJF(t_entrenador* e);
t_entrenador* RR(t_entrenador* e);

AlgoritmoFunc* getAlgoritmo(char* config);

bool desalojo;

#endif /* ALGORITMOS_H_ */

