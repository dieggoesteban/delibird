#ifndef HILOENTRENADOR_H_
#define HILOENTRENADOR_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<semaphore.h>
#include<readline/readline.h>
#include<commons/collections/list.h>

#include "utils.h"

char* IP;
char* PUERTO;
char* LOG;

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

void hiloEntrenador(uint32_t trId);

#endif /* HILOENTRENADOR_H_ */

