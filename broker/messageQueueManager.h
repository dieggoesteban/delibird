#include "global-includes.h"
#include "models.h"
#include "serializador.h"
#include "sockets.h"
#include "cache.h"

uint32_t globalMessageCounterId;

pthread_mutex_t s_counterMessageId;
pthread_mutex_t s_listaSuscriptores_newPokemon;
pthread_mutex_t s_listaSuscriptores_getPokemon;
pthread_mutex_t s_listaSuscriptores_appearedPokemon;
pthread_mutex_t s_listaSuscriptores_catchPokemon;
pthread_mutex_t s_listaSuscriptores_caughtPokemon;
pthread_mutex_t s_listaSuscriptores_localizedPokemon;

//Message Queues
t_message_queue* newPokemonMessageQueue;
t_message_queue* getPokemonMessageQueue;
t_message_queue* appearedPokemonMessageQueue;
t_message_queue* catchPokemonMessageQueue;
t_message_queue* caughtPokemonMessageQueue;
t_message_queue* localizedPokemonMessageQueue;

t_message_queue* crearMessageQueue(uint32_t mq_cod);
t_message* crearMessage(); //void* mensajeRecibido, uint32_t mq_cod
t_message_queue* getMessageQueueById(uint32_t id);
t_list* getSuscriptoresByMessageQueueId(uint32_t id);

void processMessage(t_buffer *buffer, uint32_t operation_cod, uint32_t socket_cliente);
void receiveAcknowledgement(t_acknowledgement* ack, uint32_t socket_cliente);
void subscribeNewModule(uint32_t idNewModule, uint32_t socket_cliente, uint32_t mq_cod);
void addMessageToQueue(t_message* message, t_message_queue* messageQueue);
void sendMessageFromQueue(t_message* message, t_suscripcion* suscriptor);
void dispatchMessagesFromQueue(t_message_queue* messageQueue);
void deleteFromQueue(t_message* message);
void cacheMessage(t_message* message);
void notifySender(t_message* message, uint32_t socket_cliente);
void notifySubscriber(t_suscripcion* suscripcion);

void inicializarCounterMessageId();
uint32_t asignarMessageId();

//Orden superior
uint32_t message_found(t_message* message);

//Cleaning
void freeMessage(t_message* message);
void freeMessageQueue(t_message_queue* messageQueue);