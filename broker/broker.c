#include "broker.h"

t_log* logger;
t_config* config;

//
int main(int argc, char* argv[])
{
    config = config_create("./assets/broker.config");

	logger = log_create("./assets/broker.log","broker",true,LOG_LEVEL_INFO);

    IP = config_get_string_value(config,"IP_BROKER");
    PUERTO = config_get_string_value(config,"PUERTO_BROKER");

    //Instanciación de las MQ
    newPokemonMessageQueue = crearMessageQueue(NEW_POKEMON);
   
    iniciar_servidor();

    return 0;
}

//Instancia una MQ
t_message_queue* crearMessageQueue(uint32_t mq_cod)
{
    t_message_queue* newMessageQueue = (t_message_queue*)malloc(sizeof(t_message_queue));
    newMessageQueue->mq_cod = mq_cod;
    newMessageQueue->mensajes = list_create();
    newMessageQueue->suscribers = list_create();
    return newMessageQueue;
}