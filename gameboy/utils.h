
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
#include "models.h"

#include "models.h"

#include "models.h"

t_log* logger;
t_config* config;


uint32_t procesarComando(char** ip, char** puerto, char *proceso, char *tipo_mensaje);
t_list* cortarArgumentos(int lengthArray, char *argumentos[]);
uint32_t perteneceAlArray(char* val, char* arr[], uint32_t size);
uint32_t arraySize(void* arr[]);
uint32_t arraySizeChar(char* arr[]) ;
uint32_t getColaDeMensajes(char* queue);
char* getNombreColaDeMensajes(uint32_t mq);
t_suscribe_gameboy* crearSuscribeGameboy(uint32_t con, uint32_t mq);

#endif /* UTILS_H_ */
