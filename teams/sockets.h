
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
#include<semaphore.h>
#include "serializador.h"
#include "utils.h"
#include "models.h"

t_log* logger;
t_config* config;
pthread_t thread;
char* IP;
char* PUERTO;

t_list* objetivoGlobal;
t_list* pokemonesEnMapa;

sem_t counterPokesEnMapa;
sem_t mutexPokesEnMapa;

//CLIENTE
int crear_conexion(char* ip, char* puerto);
void enviarMensaje(t_paquete* paquete, uint32_t socket_cliente);
void* recibir_paquete(int socket_cliente);
void liberar_conexion(uint32_t socket_cliente);

void sendGET();

//SERVER
void iniciar_servidor(void);
void esperar_cliente(uint32_t socket_servidor);
void serve_client(uint32_t* socket);
void process_request(uint32_t cod_op, uint32_t cliente_fd);
t_buffer* recibir_buffer(uint32_t socket_cliente);

#endif /* SOCKETS_H_ */