#ifndef GAMEBOY_H_
#define GAMEBOY_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include<pthread.h>
#include<semaphore.h>

#include "utils.h"
#include "sockets.h"
#include "models.h"

void terminarPrograma();

bool terminaConexion;
char* ipBroker;
char* puertoBroker;

uint32_t ID_MODULE;

pthread_t hiloSuscriptor;
pthread_t hiloTemporizador; //Para la suscripcion
sem_t waitForFinish;
pthread_t t_finishProgram;
t_suscribe_gameboy* suscribe;
uint32_t temp;

#endif /* GAMEBOY_H_ */
