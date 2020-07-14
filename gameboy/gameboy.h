#ifndef GAMEBOY_H_
#define GAMEBOY_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>

#include "utils.h"
#include "sockets.h"
#include "models.h"

void terminar_programa(int, t_log*, t_config*);

char* ipBroker;
char* puertoBroker;

#endif /* GAMEBOY_H_ */
