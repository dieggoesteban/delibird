
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
<<<<<<< HEAD
#include "models.h"
#include "tallgrass.h"
=======
>>>>>>> 574ee81798e3bd452a8cdf259582c2e9fd92c54e

t_log* logger;
t_config* config;
pthread_t thread;
char* IP;
char* PUERTO;

<<<<<<< HEAD
// typedef struct {
//     uint32_t mq_cod;
//     t_list* mensajes;
//     t_list* suscribers;
// } t_message_queue;

t_message_queue* newPokemonMessageQueue;
t_message_queue* appearedPokemonMessageQueue;
t_message_queue* catchPokemonMessageQueue;
t_message_queue* getPokemonMessageQueue;
t_message_queue* localizedPokemonMessageQueue;
t_message_queue* caughtPokemonMessageQueue;

=======
>>>>>>> 574ee81798e3bd452a8cdf259582c2e9fd92c54e

//CLIENTE
int crear_conexion(char* ip, char* puerto);
void enviarMensaje(t_paquete* paquete, uint32_t socket_cliente);
void* recibir_paquete(int socket_cliente);
void liberar_conexion(uint32_t socket_cliente);
<<<<<<< HEAD
uint32_t escuchaBroker();
=======
>>>>>>> 574ee81798e3bd452a8cdf259582c2e9fd92c54e

//SERVER
void iniciar_servidor(void);
void esperar_cliente(uint32_t socket_servidor);
void serve_client(uint32_t* socket);
void process_request(uint32_t cod_op, uint32_t cliente_fd);
t_buffer* recibir_buffer(uint32_t socket_cliente);

<<<<<<< HEAD
//MESSAGE QUEUE MANAGER
t_message_queue* getMessageQueueById(mq_cod id);

=======
>>>>>>> 574ee81798e3bd452a8cdf259582c2e9fd92c54e
#endif /* SOCKETS_H_ */