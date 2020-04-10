
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
#include "connection.h"

t_log* logger;
t_config* config;


// int crear_conexion(char* ip, char* puerto);
// void enviar_mensaje(char* mensaje, int socket_cliente);
// char* recibir_mensaje(int socket_cliente);
// void eliminar_paquete(t_paquete* paquete);
// void liberar_conexion(int socket_cliente);

void procesarComando(int argc, char* argv[]);
uint32_t sizeOfCharArray(uint32_t lengthArray, char* argumentos[], char* arrayTemporal[]);
uint32_t perteneceAlArray(char* val, char* arr[], uint32_t size);

#endif /* UTILS_H_ */
