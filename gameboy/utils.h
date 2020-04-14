
#ifndef UTILS_H_
#define UTILS_H_

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

t_log* logger;
t_config* config;


uint32_t procesarComando(char** ip, char** puerto, char *proceso, char *tipo_mensaje);
void cortarArgumentos(int lengthArray, char *argumentos[], char *arrayTemp[]);
uint32_t perteneceAlArray(char* val, char* arr[], uint32_t size);


#endif /* UTILS_H_ */
