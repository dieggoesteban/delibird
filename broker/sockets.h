#ifndef SOCKETS_H_
#define SOCKETS_H_

#include <errno.h>
#include "global-includes.h"
#include "models.h"
#include "serializador.h"
#include "messageQueueManager.h"

char* IP;
char* PUERTO;
pthread_t serverThread;

//CLIENTE
int crear_conexion(char* ip, char* puerto);
void enviarMensaje(t_paquete* paquete, uint32_t socket_cliente);
uint32_t enviarMensaje_returnResult(t_paquete* paquete, uint32_t socket_cliente);
void* recibir_paquete(int socket_cliente);
void liberarPaquete(t_paquete* paquete);
void liberar_conexion(uint32_t socket_cliente);

//SERVER
void iniciar_servidor(void);
void esperar_cliente(uint32_t socket_servidor);
void serve_client(uint32_t* socket);
void process_request(uint32_t cod_op, uint32_t cliente_fd);
t_buffer* recibir_buffer(uint32_t socket_cliente);

#endif /* SOCKETS_H_ */