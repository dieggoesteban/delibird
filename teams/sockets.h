
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
#include "planificador.h"
#include "utils.h"
#include "models.h"

t_log* logger;
t_config* config;

char* IP;
char* PUERTO;
bool primeraConexion;

t_list* objetivoGlobal;
t_list* pokemonesEnMapa;

t_suscribe* suscribeCaught;
t_suscribe* suscribeAppeared;
t_suscribe* suscribeLocalized;

pthread_t thread;
pthread_t threadSUSCRIBE_CAUGHT;
pthread_t threadSUSCRIBE_APPEARED;
pthread_t threadSUSCRIBE_LOCALIZED;

sem_t counterPokesEnMapa;
sem_t counterEntrenadoresCatch;
sem_t mutexPokesEnMapa;
sem_t mutexReconnect;
sem_t estaDesconectado;
sem_t detectorDesconexion;
sem_t pokesObjetivoGlobal;

uint32_t idModule;

//CLIENTE
int crear_conexion(char* ip, char* puerto);
void enviarMensaje(t_paquete* paquete, uint32_t socket_cliente);
void* recibir_paquete(int socket_cliente);
void liberar_conexion(uint32_t socket_cliente);
uint32_t escuchaBroker();
void establecerConexionBroker();
void reconectarBroker();
void desconectarBroker(uint32_t mq);

void mandarGET();
void enviarAck(t_acknowledgement* ack);

//SERVER
void iniciar_servidor(void);
void suscribe(void* message_queue);
void esperar_cliente(uint32_t socket_servidor);
void serve_client(uint32_t* socket);
void process_request(uint32_t cod_op, t_buffer* buffer, uint32_t cliente_fd);
t_buffer* recibir_buffer(uint32_t socket_cliente);

//SUSCRIPTOR
void serve_suscribe(uint32_t* socket);
void process_suscribe_request(uint32_t cod_op, t_buffer* buffer, uint32_t cliente_fd);
void detectarDesconexion();

#endif /* SOCKETS_H_ */