#ifndef MESSAGEQUEUE_H_
#define MESSAGEQUEUE_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<commons/string.h>
#include<commons/config.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include "models.h"
#include "sockets.h"

uint32_t counterMessageId;

t_message_queue* newPokemonMessageQueue;
t_message_queue* appearedPokemonMessageQueue;
t_message_queue* catchPokemonMessageQueue;
t_message_queue* getPokemonMessageQueue;
t_message_queue* localizedPokemonMessageQueue;
t_message_queue* caughtPokemonMessageQueue;

void inicializarCounterMessageId();
uint32_t asignarMessageId();
void dispatchMessage(uint32_t mq_cod, t_paquete* paquete);

#endif /*MESSAGEQUEUE_H_*/