#ifndef TEAM_H_
#define TEAM_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>

#include "sockets.h"
#include "utils.h"

char* IP;
char* PUERTO;
char* LOG;

void terminar_programa(int, t_log*, t_config*);

#endif /* TEAM_H_ */