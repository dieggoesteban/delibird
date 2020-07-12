#ifndef BROKER_H_
#define BROKER_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>

#include "utils.h"
#include "sockets.h"
#include "models.h"

char* IP;
char* PUERTO;


t_message_queue* crearMessageQueue(uint32_t mq_cod);
void terminar_programa(int, t_log*, t_config*);

#endif /* BROKER_H_ */