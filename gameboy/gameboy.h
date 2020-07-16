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

void terminar_programa(int, t_log*, t_config*);

bool terminaConexion;
char* ipBroker;
char* puertoBroker;

pthread_t hiloSuscriptor;
pthread_t hiloTemporizador; //Para la suscripcion

uint32_t temp;

#endif /* GAMEBOY_H_ */
