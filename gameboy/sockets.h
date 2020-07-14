
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
#include "serializador.h"
#include "models.h"

t_log* logger;
t_config* config;

//METODOS DE CONEXION
int crear_conexion(char* ip, char* puerto);
void enviarMensaje(t_paquete* paquete, uint32_t socket_cliente);
void suscribe(void* message_queue);
char* recibir_mensaje(int socket_cliente);
void liberar_conexion(int socket_cliente);
t_buffer *recibir_buffer(uint32_t socket_cliente);
void serve_client(uint32_t* socket);
void process_request(t_buffer *buffer, uint32_t operation_cod, uint32_t socket_cliente);

#endif /* SOCKETS_H_ */