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
uint32_t idModule;

t_list* objetivoGlobal;
t_list* pokemonesEnMapa;
t_list* entrenadoresCatch;

t_list* colaNEW;
t_list* colaREADY;
t_list* colaEXEC;
t_list* colaBLOCKED;
t_list* colaEXIT;

t_suscribe* suscribeCaught;
t_suscribe* suscribeAppeared;
t_suscribe* suscribeLocalized;

pthread_t threadREADY;
pthread_t threadEXEC;
pthread_t threadSERVER;
pthread_t threadSUSCRIBE_CAUGHT;
pthread_t threadSUSCRIBE_APPEARED;
pthread_t threadSUSCRIBE_LOCALIZED;
pthread_t threadRECONNECT;
pthread_t threadDETECT_DISCON;
pthread_t finalizarPrograma;

sem_t mutexNEW;
sem_t mutexREADY;
sem_t mutexEXEC;
sem_t mutexBLOCKED;
sem_t mutexEXIT;
sem_t mutexPokesEnMapa;
sem_t counterPokesEnMapa;
sem_t counterEntrenadoresCatch;
sem_t waitForFinish;
sem_t mutexReconnect;
sem_t mutexEntrenadoresCatch;

uint32_t cicloCPU;
uint32_t quantum;
uint32_t currentQuantum;

//INICIALIZACION Y TERMINACION DEL MODULO
void inicializarTeam();
void terminar_programa();

#endif /* TEAM_H_ */