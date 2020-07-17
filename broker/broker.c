#include "broker.h"

pthread_t serverThread;
pthread_t cacheThread;

int main() 
{
    //Inicializar parámetros
    config = config_create("./assets/broker.config");
	logger = log_create("./assets/broker.log", "broker", true, LOG_LEVEL_INFO);
    broker_custom_logger = log_create("./assets/broker_custom.log", "broker", true, LOG_LEVEL_INFO);
    IP = config_get_string_value(config, "IP_BROKER");
    PUERTO = config_get_string_value(config, "PUERTO_BROKER");

    inicializarCounterMessageId();

    //Instanciación de las colas de mensajes
    newPokemonMessageQueue = crearMessageQueue(NEW_POKEMON);
    appearedPokemonMessageQueue = crearMessageQueue(APPEARED_POKEMON);
    catchPokemonMessageQueue = crearMessageQueue(CATCH_POKEMON);
    getPokemonMessageQueue = crearMessageQueue(GET_POKEMON);
    localizedPokemonMessageQueue = crearMessageQueue(LOCALIZED_POKEMON);
    caughtPokemonMessageQueue = crearMessageQueue(CAUGHT_POKEMON);
    
    //Iniciar servidor
    pthread_create(&serverThread, NULL, (void *)iniciar_servidor, NULL);

    //Iniciar cache
    pthread_create(&cacheThread, NULL, (void *)startCache, NULL);
    
    //Activar envio de mensajes en las colas
    pthread_create(&newPokemonMessageQueue->dispatchMessagesThread, NULL, (void *)dispatchMessagesFromQueue, newPokemonMessageQueue);
    pthread_create(&appearedPokemonMessageQueue->dispatchMessagesThread, NULL, (void *)dispatchMessagesFromQueue, appearedPokemonMessageQueue);
    pthread_create(&catchPokemonMessageQueue->dispatchMessagesThread, NULL, (void *)dispatchMessagesFromQueue, catchPokemonMessageQueue);
    pthread_create(&caughtPokemonMessageQueue->dispatchMessagesThread, NULL, (void *)dispatchMessagesFromQueue, caughtPokemonMessageQueue);
    pthread_create(&getPokemonMessageQueue->dispatchMessagesThread, NULL, (void *)dispatchMessagesFromQueue, getPokemonMessageQueue);
    pthread_create(&localizedPokemonMessageQueue->dispatchMessagesThread, NULL, (void *)dispatchMessagesFromQueue, localizedPokemonMessageQueue);
    
    log_info(broker_custom_logger, "Broker inicializado correctamente...");

    //Cierre de threads
    pthread_join(serverThread, NULL);
    pthread_join(cacheThread, NULL);

    pthread_join(newPokemonMessageQueue->dispatchMessagesThread, NULL);
    pthread_join(appearedPokemonMessageQueue->dispatchMessagesThread, NULL);
    pthread_join(catchPokemonMessageQueue->dispatchMessagesThread, NULL);
    pthread_join(caughtPokemonMessageQueue->dispatchMessagesThread, NULL);
    pthread_join(getPokemonMessageQueue->dispatchMessagesThread, NULL);
    pthread_join(localizedPokemonMessageQueue->dispatchMessagesThread, NULL);

    terminarPrograma();
    return 0;
}

void terminarPrograma()
{
    log_info(broker_custom_logger, "Terminando programa...");
    free(logger);
    free(broker_custom_logger);
    free(config);
    free(newPokemonMessageQueue);
    free(appearedPokemonMessageQueue);
    free(catchPokemonMessageQueue);
    free(caughtPokemonMessageQueue);
    free(getPokemonMessageQueue);
    free(localizedPokemonMessageQueue);
    log_info(broker_custom_logger, "Programa terminado correctamente");
}