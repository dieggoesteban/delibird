#include "cache.h"

void startCache()
{
    TAMANO_MEMORIA = (uint32_t)atoi(config_get_string_value(config, "TAMANO_MEMORIA"));
    TAMANO_MINIMO_PARTICION = (uint32_t)atoi(config_get_string_value(config, "TAMANO_MINIMO_PARTICION"));
    ALGORITMO_MEMORIA =  config_get_string_value(config, "ALGORITMO_MEMORIA");
    ALGORITMO_REEMPLAZO = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
    ALGORITMO_PARTICION_LIBRE = config_get_string_value(config, "ALGORITMO_PARTICION_LIBRE");
    FRECUENCIA_COMPACTACION = (uint32_t)atoi(config_get_string_value(config, "FRECUENCIA_COMPACTACION"));

    #pragma region Definición de algoritmos

    if(strcmp(ALGORITMO_MEMORIA, "BS") == 0)
        algoritmo_memoria = memoria_buddySystem;
    else if(strcmp(ALGORITMO_MEMORIA, "PARTICIONES") == 0)
        algoritmo_memoria = memoria_particiones;

    if(strcmp(ALGORITMO_REEMPLAZO, "FIFO") == 0)
        algoritmo_reemplazo = reemplazo_fifo;
    else if(strcmp(ALGORITMO_REEMPLAZO, "LRU") == 0)
        algoritmo_reemplazo = reemplazo_lru;

    if(strcmp(ALGORITMO_PARTICION_LIBRE, "FF") == 0)
        algoritmo_particion_libre = particionLibre_ff;
    else if(strcmp(ALGORITMO_PARTICION_LIBRE, "BF") == 0)
        algoritmo_particion_libre = particionLibre_bf;

    #pragma endregion

    //Initial params
    counterToCompactacion = 0;
    partitionCounter = 0;

    partitions = list_create();
    holes = list_create();
    metadatas = list_create();

    if(pthread_mutex_init(&s_cache, NULL) != 0)
        log_error(broker_custom_logger, "Error mutex init (s_cache) in cache initialization");
    if(pthread_mutex_init(&s_partitionCounter, NULL) != 0)
		log_error(broker_custom_logger, "Error mutex init (s_partitionCounter) in cache initialization");
    if(pthread_mutex_init(&s_counterToCompactacion, NULL) != 0)
		log_error(broker_custom_logger, "Error mutex init (s_counterToCompactacion) in cache initialization");
    if(pthread_mutex_init(&s_partitions, NULL) != 0)
	    log_error(broker_custom_logger, "Error mutex init (s_partitions) in cache initialization");
    if(pthread_mutex_init(&s_holes, NULL) != 0)
		log_error(broker_custom_logger, "Error mutex init (s_holes) in cache initialization");
    if(pthread_mutex_init(&s_metadatas, NULL) != 0)
	    log_error(broker_custom_logger, "Error mutex init (s_metadatas) in cache initialization");
       
    //Memory allocation
    cache = malloc(TAMANO_MEMORIA);
    t_holes* initial = createHole(cache, TAMANO_MEMORIA);
    list_add(holes, initial);

    #pragma region Prueba de consolidacion

        // t_holes* initialPart1 = createHole(cache, 1024);
        // t_holes* initialPart2 = createHole(initialPart1->pLimit + 1, 1024);
        // t_holes* initialPart3 = createHole(initialPart2->pLimit + 1, 512);
        // t_holes* initialPart4 = createHole(initialPart3->pLimit + 12, 3096);

        // list_add(holes, initialPart1);
        // list_add(holes, initialPart2);
        // list_add(holes, initialPart3);
        // list_add(holes, initialPart4);
        // showHoles();
        // consolidar();
        // printf("\n");
        // showHoles();
        // log_info(logger, "Cache module has started successfully\n");

        // free(initial);
        // free(cache);
        // free(initialPart1);
        // free(initialPart2);
    
    #pragma endregion
    #pragma region Prueba de compactacion
    /*
    
        t_partition* partition1 = createPartition(cache, 1024);
        t_holes* initialPart1 = createHole(partition1->pLimit + 1, 512);
        t_partition* partition2 = createPartition(initialPart1->pLimit + 1, 256);
        t_holes* initialPart2 = createHole(partition2->pLimit + 1, 512);
        t_partition* partition3 = createPartition(initialPart2->pLimit + 1, 128);
        t_holes* initialPart3 = createHole(partition3->pLimit + 1, 512);

        list_add(partitions, partition1);
        list_add(partitions, partition2);
        list_add(partitions, partition3);
        list_add(holes, initialPart1);
        list_add(holes, initialPart2);
        list_add(holes, initialPart3);

        compactar();

        showPartitions();
        showHoles();*/
    #pragma endregion
    
}

void memoria_buddySystem(t_message* message) 
{
    log_info(broker_custom_logger, "I'm using Buddy System Algorithm\n");

    //not implemented
}

void memoria_particiones(t_message* message) 
{
    //log_info(broker_custom_logger, "Dynamic Partitions Algorithm\n");
    cache_message* administrative = (cache_message*)malloc(sizeof(cache_message));
    void* addressFromMessageToCopy;
    uint32_t sizeOfMessage;

    switch (message->mq_cod)
    {
        case NEW_POKEMON:
        {
            t_new_pokemon* newPoke = (t_new_pokemon*)message->mensaje;
            cache_new_pokemon cache_newPoke;

            cache_newPoke.nameLength = newPoke->sizeNombre;
            cache_newPoke.pokeName = malloc(newPoke->sizeNombre);
            cache_newPoke.cantidad = newPoke->posicionCantidad->cantidad;
            cache_newPoke.posX = newPoke->posicionCantidad->posicion_x;
            cache_newPoke.posY = newPoke->posicionCantidad->posicion_y;
            memcpy(cache_newPoke.pokeName, newPoke->nombre, newPoke->sizeNombre);
            
            administrative->idMessage = newPoke->ID_mensaje_recibido;
            administrative->mq_cod = NEW_POKEMON;
            administrative->length = (sizeof(uint32_t) * 4 + (cache_newPoke.nameLength - 1));
            administrative->suscriptoresEnviados = list_duplicate(message->suscriptoresEnviados);
            administrative->suscriptoresConfirmados = list_duplicate(message->suscriptoresConfirmados);
            
            addressFromMessageToCopy = &cache_newPoke;
            sizeOfMessage = administrative->length;
            break;
        }
        case APPEARED_POKEMON:
        {
            t_appeared_pokemon* appearedPoke = (t_appeared_pokemon*)message->mensaje;
            cache_appeared_pokemon cache_appeared;

            cache_appeared.nameLength = appearedPoke->sizeNombre;            
            cache_appeared.pokeName = malloc(appearedPoke->sizeNombre);
            cache_appeared.posX = appearedPoke->posicion->posicion_x;
            cache_appeared.posY = appearedPoke->posicion->posicion_y;
            strcpy(cache_appeared.pokeName, appearedPoke->nombre);
            
            administrative->idMessage = appearedPoke->ID_mensaje_recibido;
            administrative->idCorrelational = appearedPoke->ID_mensaje_original;
            administrative->mq_cod = APPEARED_POKEMON;
            administrative->length = sizeof(u_int32_t) * 3 + (cache_appeared.nameLength - 1);
            administrative->suscriptoresEnviados = list_duplicate(message->suscriptoresEnviados);
            administrative->suscriptoresConfirmados = list_duplicate(message->suscriptoresConfirmados);

            addressFromMessageToCopy = &appearedPoke;
            sizeOfMessage = administrative->length;
            break;
        }
        case CATCH_POKEMON:
        {
            t_catch_pokemon* catchPoke = (t_catch_pokemon*)message->mensaje;
            cache_catch_pokemon cache_catch;

            cache_catch.nameLength = catchPoke->sizeNombre;            
            cache_catch.pokeName = malloc(catchPoke->sizeNombre);
            cache_catch.posX = catchPoke->posicion->posicion_x;
            cache_catch.posY = catchPoke->posicion->posicion_y;
            strcpy(cache_catch.pokeName, catchPoke->nombre);
            
            administrative->idMessage = catchPoke->ID_mensaje_recibido;
            administrative->mq_cod = CATCH_POKEMON;
            administrative->length = sizeof(u_int32_t) * 3 + (cache_catch.nameLength - 1);
            administrative->suscriptoresEnviados = list_duplicate(message->suscriptoresEnviados);
            administrative->suscriptoresConfirmados = list_duplicate(message->suscriptoresConfirmados);

            addressFromMessageToCopy = &cache_catch;
            sizeOfMessage = administrative->length;
            break;
        }
        case CAUGHT_POKEMON:
        {
            t_caught_pokemon* caughtPoke = (t_caught_pokemon*)message->mensaje;
            cache_caught_pokemon cache_caught;

            cache_caught.seAtrapo = caughtPoke->catchStatus;
            
            administrative->idMessage = caughtPoke->ID_mensaje_recibido;
            administrative->idCorrelational = caughtPoke->ID_mensaje_original;
            administrative->mq_cod = CAUGHT_POKEMON;
            administrative->length = sizeof(uint32_t);
            administrative->suscriptoresEnviados = list_duplicate(message->suscriptoresEnviados);
            administrative->suscriptoresConfirmados = list_duplicate(message->suscriptoresConfirmados);

            addressFromMessageToCopy = &cache_caught;
            sizeOfMessage = administrative->length;
            break;
        }
        case GET_POKEMON:
        {
            t_get_pokemon* getPoke = (t_get_pokemon*)message->mensaje;
            cache_get_pokemon cache_getPoke;

            cache_getPoke.nameLength = getPoke->sizeNombre;
            cache_getPoke.pokeName = malloc(getPoke->sizeNombre);
            memcpy(cache_getPoke.pokeName, getPoke->nombre, getPoke->sizeNombre);
            
            administrative->idMessage = getPoke->ID_mensaje_recibido;
            administrative->mq_cod = GET_POKEMON;
            administrative->length = (sizeof(uint32_t) + (cache_getPoke.nameLength - 1));
            administrative->suscriptoresEnviados = list_duplicate(message->suscriptoresEnviados);
            administrative->suscriptoresConfirmados = list_duplicate(message->suscriptoresConfirmados);
            
            addressFromMessageToCopy = &cache_getPoke;
            sizeOfMessage = administrative->length;
            break;
        }
        case LOCALIZED_POKEMON: 
        {
            // t_localized_pokemon* localizedPoke = (t_localized_pokemon*)message->mensaje;
            // cache_localized_pokemon cache_localizedPoke;

            // cache_localizedPoke.nameLength = localizedPoke->sizeNombre;
            // cache_localizedPoke.pokeName = malloc(localizedPoke->sizeNombre);
            // cache_localizedPoke.posiciones = localizedPoke->cantidadPosiciones;

            // memcpy(cache_localizedPoke.pokeName, localizedPoke->nombre, localizedPoke->sizeNombre);
            
            // administrative->idMessage = localizedPoke->ID_mensaje_recibido;
            // administrative->mq_cod = LOCALIZED_POKEMON;
            // administrative->length = (sizeof(uint32_t) * 3 + (cache_localizedPoke.nameLength - 1));
            // administrative->suscriptoresEnviados = list_duplicate(message->suscriptoresEnviados);
            // administrative->suscriptoresConfirmados = list_duplicate(message->suscriptoresConfirmados);
            
            // addressFromMessageToCopy = &localizedPoke;
            // sizeOfMessage = administrative->length;
            break;
        }
        default:
            log_error(broker_custom_logger, "Codigo de operación desconocido. No se puede cachear el mensaje");
    }

    //Etapa 1: Buscar partición libre
    t_holes* targetHole = algoritmo_particion_libre(sizeOfMessage); //Me asegura que de ese hole puedo sacar la particion minima por config
    while(targetHole == NULL)
    {
        pthread_mutex_lock(&s_counterToCompactacion);
        if(counterToCompactacion < FRECUENCIA_COMPACTACION)
        {
            counterToCompactacion++;
        }
        else
        {           
            compactar();
            counterToCompactacion = 0;
            targetHole = algoritmo_particion_libre(sizeOfMessage);
            pthread_mutex_unlock(&s_counterToCompactacion);
            continue;
        }
        pthread_mutex_unlock(&s_counterToCompactacion);
        
        //Etapa 3: Reemplazo y Consolidacion
        targetHole = algoritmo_reemplazo(sizeOfMessage);
        pthread_mutex_lock(&s_holes);
            consolidar();
        pthread_mutex_unlock(&s_holes);
    }
    
    administrative->startAddress = targetHole->pStart;

    //Guardamos la estructura administrativa
    pthread_mutex_lock(&s_metadatas);
        list_add(metadatas, (void*)administrative);
    pthread_mutex_unlock(&s_metadatas);

    //Cuando el targetPartition ya esté ubicado se meten los datos donde corresponda
    writeData(administrative, targetHole, addressFromMessageToCopy);
}

void writeData(cache_message* administrative, t_holes* targetHole, void* message)
{
    t_partition* partition = createPartition(targetHole->pStart, targetHole->length);
    partition->id = administrative->idMessage;

    pthread_mutex_lock(&s_partitions);
        list_add(partitions, (void*)partition);
        list_sort(partitions, (void*)mem_address_menor_a_mayor);
    pthread_mutex_unlock(&s_partitions);
    
    pthread_mutex_lock(&s_cache);
        memcpy(administrative->startAddress, message, administrative->length);
    pthread_mutex_unlock(&s_cache);
}

t_holes* reemplazo_fifo(uint32_t bytes)
{   
    t_partition* targetFifoPartition = NULL;

    //Se crea una copia para ordenarla por fifo
    t_list* tempPartitionsList = list_duplicate(partitions);
    uint32_t _fifo_menor_a_mayor (t_partition* partition1, t_partition* partition2) {
        return partition1->fifoPosition < partition2->fifoPosition;
    }
    list_sort(tempPartitionsList, (void*)_fifo_menor_a_mayor);

    while (targetFifoPartition == NULL || targetFifoPartition->length < bytes)
    {
        //Se toma el numero de fifo de la primera particion despues del orden
        t_partition* targetFifoPartition = (t_partition*)list_get(tempPartitionsList, 0);
        if(targetFifoPartition->length < bytes)
        {
            list_remove(tempPartitionsList, 0);
            continue;
        }
        uint32_t fifoToDelete = targetFifoPartition->fifoPosition;

        free(targetFifoPartition);

        //Se ubica a la victima en la lista original y se crea el nuevo hueco con sus datos
        uint32_t _is_the_one_with_fifo_number(t_partition* partition) {
            return partition->fifoPosition == fifoToDelete;
        }

        pthread_mutex_lock(&s_partitions);
            t_partition* victim = (t_partition*)list_find(partitions, (void*)_is_the_one_with_fifo_number);
        pthread_mutex_unlock(&s_partitions);

        if(victim == NULL) {
            log_error(broker_custom_logger, "No se encontro particion victima");
            exit(1);
        }
        t_holes* newFreeSpace = createHole(victim->pStart, victim->length);
        
        //Se elimina de la lista de particiones
        t_partition* currentPartition;
        pthread_mutex_lock(&s_partitions);
        for(uint32_t i = 0; i < list_size(partitions); i++)
            {
                currentPartition = list_get(partitions, i);
                if(currentPartition->fifoPosition == victim->fifoPosition)
                {
                    list_remove(partitions, i);
                    break;
                }
            }
        pthread_mutex_unlock(&s_partitions);

        return newFreeSpace;
    }
    return NULL;
}

t_holes* reemplazo_lru(uint32_t bytes){ return createHole(0, 0); }

void dispatchCachedMessages(t_cache_dispatch_info* dispatchInfo)
{
    switch (dispatchInfo->mq_cod)
    {
        case NEW_POKEMON:
        {

            break;
        }
        case APPEARED_POKEMON:
        {

            break;
        }
    }
}

void dump()
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    //Añadir locks
    t_list* allMemory = list_create();

    pthread_mutex_lock(&s_partitions);
        list_add_all(allMemory, partitions);
    pthread_mutex_unlock(&s_partitions);
    pthread_mutex_lock(&s_holes);
        list_add_all(allMemory, holes);
    pthread_mutex_unlock(&s_holes);

    list_sort(allMemory, (void*)mem_address_menor_a_mayor);

    printf("\nDump: %02d/%02d/%d %02d:%02d:%02d\n",
        tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
    t_partition* currentBlock;
    cache_message* currentMetadata;
    
    for (uint32_t i = 0; i < list_size(allMemory); i++)
    {
        currentBlock = (t_partition*)list_get(allMemory, i);
        uint32_t _is_the_metadata(cache_message* message) {
            return message->idMessage == currentBlock->id;
        }

        //Si es una particion ocupada
        if(currentBlock->free == 'X')
        {
            currentMetadata = (cache_message*)list_find(metadatas, (void*)_is_the_metadata);
            printf("Particion %i:  %p - %p    [%c]    Size: %ib    LRU: %llu    COLA: %i   ID: %i\n", 
                i+1, currentBlock->pStart, currentBlock->pLimit, currentBlock->free, currentBlock->length, currentBlock->lastUse
                , currentMetadata->mq_cod, currentMetadata->idMessage);
        }
        else
        {
            printf("Particion %i:  %p - %p    [%c]    Size: %ib    LRU: -    COLA: -   ID: -\n", 
                i+1, currentBlock->pStart, currentBlock->pLimit, currentBlock->free, currentBlock->length);
        }

    }
}

void consolidar()
{
    if(list_size(holes) == 0 || list_size(holes) == 1)
    {
        log_info(broker_custom_logger, "Hay un solo uno o ningun hueco, no es necesario consolidar");
    }

    t_holes* lowerHole;
    t_holes* upperHole;

    uint32_t i = 0;
    while (list_size(holes) >= 2 && i < list_size(holes) - 1)
    {        
        lowerHole = list_get(holes, i);
        upperHole = list_get(holes, i + 1);

        if(lowerHole->pLimit == upperHole->pStart - 1)
        {
            //Inicia la Consolidacion
            t_holes* consolidado = createHole(lowerHole->pStart, lowerHole->length + upperHole->length);

            //Se elimina la posición i dos veces porque el espacio contiguo se desplaza un lugar
            pthread_mutex_lock(&s_holes);
                list_remove(holes, i);
                list_remove(holes, i);
                list_add(holes, consolidado);
                //Ordenamos para siempre comparar los primeros 2 holes
                list_sort(holes, (void*) mem_address_menor_a_mayor);
            pthread_mutex_unlock(&s_holes);
            i = 0;
            continue;
        }
        i++;
    }
}

void compactar()
{
    if(list_size(partitions) == 0 || list_size(holes) == 0)
    {
        log_info(broker_custom_logger, "No hay particiones o no hay huecos -> No es necesario compactar");
        pthread_mutex_unlock(&s_counterToCompactacion);
        return;
    }

    //Primero asegurar que no haya huecos contiguos y luego ordenar ambas listas
    consolidar();
    list_sort(partitions, (void*) mem_address_menor_a_mayor);
    list_sort(holes, (void*) mem_address_menor_a_mayor);
    uint32_t targetPartitionIndex;
    t_partition* targetPartition;

    pthread_mutex_lock(&s_holes);
    pthread_mutex_lock(&s_partitions);
    while (existHolesBetweenPartitions())
    {
        t_holes* hole = (t_holes*)list_get(holes, 0);

        //Busco la particion contigua al hueco
        targetPartitionIndex = -1;
        for(uint32_t i = 0; i < list_size(partitions); i++)
        {
            targetPartition = list_get(partitions, i);
            if(targetPartition->pStart == hole->pLimit + 1)
            {
                targetPartitionIndex = i;
                break;
            }
        }

        if(targetPartition == NULL) //No hay particiones continuas a ese hueco
            return;
        
        //Se hace un swap entre la particion y el hueco
        t_partition* relocatedPartition = createPartition(hole->pStart, targetPartition->length);
        t_holes* relocatedHole = createHole(relocatedPartition->pLimit + 1, hole->length);

        list_remove(holes, 0);
        list_remove(partitions, targetPartitionIndex);
        list_add(holes, relocatedHole);
        list_add(partitions, relocatedPartition);
        list_sort(partitions, (void*) mem_address_menor_a_mayor);
        list_sort(holes, (void*) mem_address_menor_a_mayor);
        consolidar();

        targetPartition = NULL;
    }
    pthread_mutex_unlock(&s_holes);
    pthread_mutex_unlock(&s_partitions);
}

t_holes* particionLibre_ff(uint32_t bytes) 
{ 
    uint32_t bytesToAlloc = 0;
    if(bytes > TAMANO_MINIMO_PARTICION)
        bytesToAlloc = bytes;
    else
        bytesToAlloc = TAMANO_MINIMO_PARTICION;

    t_holes* result = NULL;
    t_holes* currentHole = NULL;
    pthread_mutex_lock(&s_holes);
        for(int i = 0; i < list_size(holes); i++)
        {
            currentHole = list_get(holes, i);
            if (currentHole->length > bytesToAlloc)
            {
                //Hacer el split
                t_holes* splittedHole = createHole(currentHole->pStart + bytesToAlloc, (currentHole->length - bytesToAlloc));
                list_add(holes, (void*)splittedHole);
                result = createHole(currentHole->pStart, bytesToAlloc);
                t_holes* holeToDelete = list_remove(holes, i);
                free(holeToDelete);
                //list_sort(holes, (void*)mem_address_menor_a_mayor);
                break;
            }
            else if (currentHole->length == bytesToAlloc)
            {
                result = createHole(currentHole->pStart, bytesToAlloc);
                t_holes* holeToDelete = list_remove(holes, i);
                free(holeToDelete);
                break;
            }
        }
        list_sort(holes, (void*)mem_address_menor_a_mayor);
    pthread_mutex_unlock(&s_holes);
    
    return result;
}

t_holes* particionLibre_bf(uint32_t bytes)
{
    uint32_t bytesToAlloc = 0;
    if(bytes > TAMANO_MINIMO_PARTICION)
        bytesToAlloc = bytes;
    else
        bytesToAlloc = TAMANO_MINIMO_PARTICION;

    void* result = NULL;
    t_holes* currentHole;
    t_holes* bestHole;

    pthread_mutex_lock(&s_holes);
        for(uint32_t i = 0; i < list_size(holes); i++)
        {
            currentHole = list_get(holes, i);
            if(bestHole == NULL && currentHole->length >= bytesToAlloc)
                bestHole = currentHole;
            else if (currentHole->length >= bytesToAlloc && currentHole->length < bestHole->length)
            {
                bestHole = currentHole;
                if(bestHole->length == bytesToAlloc)
                    break;
            }      
        }
    pthread_mutex_unlock(&s_holes);

    result = bestHole;
    return result;
}


//AUX Methods
t_holes* createHole(void* startAddress, uint32_t length)
{
    t_holes* hole = (t_holes*)malloc(sizeof(t_holes));
    hole->pStart = startAddress;
    hole->pLimit = startAddress + length - 1; //Se resta uno porque hay que incluir el byte de pStart
    hole->length = length; 
    hole->free = 'L';
    hole->lastUse = getTimestamp();
    return hole;
}
void showHoles()
{
    t_holes* currentHole;

    pthread_mutex_lock(&s_holes);
    for(uint32_t i = 0; i < list_size(holes); i++)
    {
        currentHole = list_get(holes, i);
        printf("Hole %d - Range: %p / %p - Length: %d\n", i, 
            currentHole->pStart, currentHole->pLimit, currentHole->length);
    }
    pthread_mutex_unlock(&s_holes);
}

t_partition* createPartition(void* startAddress, uint32_t length)
{
    t_partition* partition = (t_partition*)malloc(sizeof(t_partition));
    partition->pStart = startAddress;
    partition->pLimit = startAddress + length - 1; //Se resta uno porque hay que incluir el byte de pStart
    partition->length = length; 
    partition->free = 'X';
    partition->lastUse = getTimestamp();

    pthread_mutex_lock(&s_partitionCounter);
        partition->fifoPosition = ++partitionCounter;
    pthread_mutex_unlock(&s_partitionCounter);
    return partition;
}
void showPartitions()
{
    t_partition* currentPartition;

    pthread_mutex_lock(&s_partitions);
    for(uint32_t i = 0; i < list_size(partitions); i++)
    {
        currentPartition = list_get(partitions, i);
        printf("Partition %d - Range: %p / %p - Length: %d\n", i, 
            currentPartition->pStart, currentPartition->pLimit, currentPartition->length);
    }
    pthread_mutex_unlock(&s_partitions);
}

uint32_t mem_address_menor_a_mayor(t_holes* block, t_holes* block2)
{
    return block->pStart < block2->pStart;
}

// uint32_t timestamp_menor_a_mayor(t_partition* block, t_partition* block2)
// {
//     return block->lastUse < block2->lastUse;
// }

uint32_t existHolesBetweenPartitions()
{
    t_holes* firstHole = list_get(holes, 0); //El primer hueco
    t_partition* lastPartition = list_get(partitions, list_size(partitions) - 1); //La ultima particion

    return firstHole->pStart < lastPartition->pLimit;
}

uint64_t getTimestamp()
{
    struct timeval valor;
    gettimeofday(&valor, NULL);
    unsigned long long result = (((unsigned long long )valor.tv_sec) * 1000 + ((unsigned long) valor.tv_usec));
    uint64_t tiempo = result;
    return tiempo;
}