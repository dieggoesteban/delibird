#include "global-includes.h"
#include "models.h"
#include "serializador.h"
#include "sockets.h"

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
t_message* crearMessage(void* mensajeRecibido, uint32_t mq_cod);
t_message_queue* getMessageQueueById(uint32_t id);
t_list* getSuscriptoresByMessageQueueId(uint32_t id);

void processMessage(t_buffer *buffer, uint32_t operation_cod, uint32_t socket_cliente);
void subscribeNewModule(uint32_t id, uint32_t mq_cod);
void addMessageToMQ(t_message* message, t_message_queue* messageQueue);
void sendMessageFromQueue(t_message* message, uint32_t socket_cliente);
void dispatchMessagesFromMQ(t_message_queue* messageQueue);
void cacheMessage(t_message* message);
void notifySender(t_message* message, uint32_t socket_cliente);

void inicializarCounterMessageId();
uint32_t asignarMessageId();