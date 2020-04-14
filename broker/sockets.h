
#ifndef SOCKETS_H_
#define SOCKETS_H_

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
#include<pthread.h>
#include "connection.h"

t_log* logger;
t_config* config;
pthread_t thread;

//CLIENTE
int crear_conexion(char* ip, char* puerto);
void enviarMensaje(t_paquete* paquete, uint32_t socket_cliente);
void* recibir_paquete(int socket_cliente);
void liberar_conexion(int socket_cliente);

//SERVER


#endif /* SOCKETS_H_ */