#include "broker.h"
#include <signal.h>

pthread_t serverThread;
pthread_t cacheThread;

void finish()
{
    sem_post(&waitForFinish);
}

int main() 
{
    signal(SIGINT, finish); 
    signal(SIGUSR1, dump);
    
    sem_init(&waitForFinish, 0, 0);
    
    //Inicializar parámetros
    remove("./dumps/cache_dump.txt");
    remove("./assets/broker.log");
    remove("./assets/broker_custom.log");
    
    config = config_create("./assets/broker.config");
    LOG = config_get_string_value(config,"LOG_FILE");
	logger = log_create(LOG, "broker", true, LOG_LEVEL_INFO);
    broker_custom_logger = log_create("./assets/broker_custom.log", "broker", true, LOG_LEVEL_DEBUG);
    IP = config_get_string_value(config, "IP_BROKER");
    PUERTO = config_get_string_value(config, "PUERTO_BROKER");

    log_debug(broker_custom_logger, "PID de Broker %i", getpid());

    inicializarCounterMessageId();

    //Instanciación de las colas de mensajes
    newPokemonMessageQueue = crearMessageQueue(NEW_POKEMON);
    appearedPokemonMessageQueue = crearMessageQueue(APPEARED_POKEMON);
    catchPokemonMessageQueue = crearMessageQueue(CATCH_POKEMON);
    getPokemonMessageQueue = crearMessageQueue(GET_POKEMON);
    localizedPokemonMessageQueue = crearMessageQueue(LOCALIZED_POKEMON);
    caughtPokemonMessageQueue = crearMessageQueue(CAUGHT_POKEMON);
    
    pthread_create(&t_finishProgram, NULL, (void*)terminarPrograma, NULL);
    pthread_detach(t_finishProgram);

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

    return 0;
}

void terminarPrograma()
{
    sem_wait(&waitForFinish);
    
    log_info(broker_custom_logger, "Finalizando broker...");

    freeMessageQueue(newPokemonMessageQueue);
    freeMessageQueue(appearedPokemonMessageQueue);
    freeMessageQueue(catchPokemonMessageQueue);
    freeMessageQueue(caughtPokemonMessageQueue);
    freeMessageQueue(getPokemonMessageQueue);
    freeMessageQueue(localizedPokemonMessageQueue);
    freeCacheSystem();

    log_info(broker_custom_logger, "Broker finalizado correctamente");
    config_destroy(config);
    log_destroy(logger);
    log_destroy(broker_custom_logger);
    exit(0);
}