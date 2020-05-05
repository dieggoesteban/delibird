
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
#include "serializador.h"
#include "models.h"

t_log* logger;
t_config* config;
pthread_t thread;
char* IP;
char* PUERTO;

// typedef struct {
//     uint32_t mq_cod;
//     t_list* mensajes;
//     t_list* suscribers;
// } t_message_queue;

t_message_queue* newPokemonMessageQueue;
t_message_queue* appearedPokemonMessageQueue;


//CLIENTE
int crear_conexion(char* ip, char* puerto);
void enviarMensaje(t_paquete* paquete, uint32_t socket_cliente);
void* recibir_paquete(int socket_cliente);
void liberar_conexion(uint32_t socket_cliente);

//SERVER
void iniciar_servidor(void);
void esperar_cliente(uint32_t socket_servidor);
void serve_client(uint32_t* socket);
void process_request(uint32_t cod_op, uint32_t cliente_fd);
t_buffer* recibir_buffer(uint32_t socket_cliente);

//MESSAGE QUEUE MANAGER
t_message_queue* getMessageQueueById(mq_cod id);

#endif /* SOCKETS_H_ */