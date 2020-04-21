#ifndef GAMECARD_H_
#define GAMECARD_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>

#include "utils.h"
#include "sockets.h"

char* IP;
char* PUERTO;

void terminar_programa(int, t_log*, t_config*);

#endif /* GAMECARD_H_ */