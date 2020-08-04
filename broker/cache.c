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

    // sem_init(&s_cache, 0, 1);
    // sem_init(&s_partitionCounter, 0, 1);
    // sem_init(&s_counterToCompactacion, 0, 1);
    // sem_init(&s_partitions, 0, 1);
    // sem_init(&s_holes, 0, 1);
    // sem_init(&s_metadatas, 0, 1);


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
}

void memoria_buddySystem(t_message* message) 
{
    log_warning(broker_custom_logger, "Buddy system no implementado para el grupo Socketes por disposición de la cátedra");    
    //not implemented
}

void memoria_particiones(t_message* message) 
{
    cache_message* administrative = (cache_message*)malloc(sizeof(cache_message));
    t_cache_buffer* addressFromMessageToCopy;

    switch (message->mq_cod)
    {
        case NEW_POKEMON:
        {
            t_new_pokemon* newPoke = (t_new_pokemon*)message->mensaje;
            cache_new_pokemon cache_newPoke;

            cache_newPoke.nameLength = newPoke->sizeNombre - 1;
            cache_newPoke.pokeName = malloc(cache_newPoke.nameLength);
            cache_newPoke.cantidad = newPoke->posicionCantidad->cantidad;
            cache_newPoke.posX = newPoke->posicionCantidad->posicion_x;
            cache_newPoke.posY = newPoke->posicionCantidad->posicion_y;
            memcpy(cache_newPoke.pokeName, string_substring(newPoke->nombre, 0, cache_newPoke.nameLength), cache_newPoke.nameLength);
            
            administrative->idMessage = newPoke->ID_mensaje_recibido;
            administrative->idCorrelational = 0;
            administrative->mq_cod = NEW_POKEMON;
            administrative->suscriptoresEnviados = list_duplicate(message->suscriptoresEnviados);
            administrative->suscriptoresConfirmados = list_duplicate(message->suscriptoresConfirmados);
            
            t_cache_buffer* buffer = serializar_cacheNewPokemon(&cache_newPoke);
            addressFromMessageToCopy = buffer;
            administrative->length = buffer->size;
            free(cache_newPoke.pokeName);
            break;
        }
        case APPEARED_POKEMON:
        {
            t_appeared_pokemon* appearedPoke = (t_appeared_pokemon*)message->mensaje;
            cache_appeared_pokemon cache_appeared;

            cache_appeared.nameLength = appearedPoke->sizeNombre - 1;            
            cache_appeared.pokeName = malloc(cache_appeared.nameLength);
            cache_appeared.posX = appearedPoke->posicion->posicion_x;
            cache_appeared.posY = appearedPoke->posicion->posicion_y;
            memcpy(cache_appeared.pokeName, string_substring(appearedPoke->nombre, 0, cache_appeared.nameLength), cache_appeared.nameLength);
            
            administrative->idMessage = appearedPoke->ID_mensaje_recibido;
            administrative->idCorrelational = appearedPoke->ID_mensaje_original;
            administrative->mq_cod = APPEARED_POKEMON;
            administrative->suscriptoresEnviados = list_duplicate(message->suscriptoresEnviados);
            administrative->suscriptoresConfirmados = list_duplicate(message->suscriptoresConfirmados);

            t_cache_buffer* buffer = serializar_cacheAppearedPokemon(&cache_appeared);
            addressFromMessageToCopy = buffer;
            administrative->length = buffer->size;
            free(cache_appeared.pokeName);
            break;
        }
        case CATCH_POKEMON:
        {
            t_catch_pokemon* catchPoke = (t_catch_pokemon*)message->mensaje;
            cache_catch_pokemon cache_catch;

            cache_catch.nameLength = catchPoke->sizeNombre - 1;            
            cache_catch.pokeName = malloc(cache_catch.nameLength);
            cache_catch.posX = catchPoke->posicion->posicion_x;
            cache_catch.posY = catchPoke->posicion->posicion_y;
            memcpy(cache_catch.pokeName, string_substring(catchPoke->nombre, 0, cache_catch.nameLength), cache_catch.nameLength);
            
            administrative->idMessage = catchPoke->ID_mensaje_recibido;
            administrative->idCorrelational = 0;
            administrative->mq_cod = CATCH_POKEMON;
            administrative->suscriptoresEnviados = list_duplicate(message->suscriptoresEnviados);
            administrative->suscriptoresConfirmados = list_duplicate(message->suscriptoresConfirmados);

            t_cache_buffer* buffer = serializar_cacheCatchPokemon(&cache_catch);
            addressFromMessageToCopy = buffer;
            administrative->length = buffer->size;
            free(cache_catch.pokeName);
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
            administrative->suscriptoresEnviados = list_duplicate(message->suscriptoresEnviados);
            administrative->suscriptoresConfirmados = list_duplicate(message->suscriptoresConfirmados);

            t_cache_buffer* buffer = serializar_cacheCaughtPokemon(&cache_caught);
            addressFromMessageToCopy = buffer;
            administrative->length = buffer->size;
            break;
        }
        case GET_POKEMON:
        {
            t_get_pokemon* getPoke = (t_get_pokemon*)message->mensaje;
            cache_get_pokemon cache_getPoke;

            cache_getPoke.nameLength = getPoke->sizeNombre - 1;
            cache_getPoke.pokeName = malloc(cache_getPoke.nameLength);
            memcpy(cache_getPoke.pokeName, string_substring(getPoke->nombre, 0, cache_getPoke.nameLength), cache_getPoke.nameLength);
            
            administrative->idMessage = getPoke->ID_mensaje_recibido;
            administrative->idCorrelational = 0;
            administrative->mq_cod = GET_POKEMON;
            administrative->suscriptoresEnviados = list_duplicate(message->suscriptoresEnviados);
            administrative->suscriptoresConfirmados = list_duplicate(message->suscriptoresConfirmados);
            
            t_cache_buffer* buffer = serializar_cacheGetPokemon(&cache_getPoke);
            addressFromMessageToCopy = buffer;
            administrative->length = buffer->size;
            free(cache_getPoke.pokeName);
            break;
        }
        case LOCALIZED_POKEMON: 
        {
            t_localized_pokemon* localizedPoke = (t_localized_pokemon*)message->mensaje;
            cache_localized_pokemon cache_localizedPoke;

            cache_localizedPoke.nameLength = localizedPoke->sizeNombre - 1;
            cache_localizedPoke.pokeName = malloc(cache_localizedPoke.nameLength);
            cache_localizedPoke.cantParesCoords = localizedPoke->cantidadPosiciones;
            memcpy(cache_localizedPoke.pokeName, string_substring(localizedPoke->nombre, 0, cache_localizedPoke.nameLength), cache_localizedPoke.nameLength);
            
            administrative->idMessage = localizedPoke->ID_mensaje_recibido;
            administrative->idCorrelational = localizedPoke->ID_mensaje_original;
            administrative->mq_cod = LOCALIZED_POKEMON;
            administrative->suscriptoresEnviados = list_duplicate(message->suscriptoresEnviados);
            administrative->suscriptoresConfirmados = list_duplicate(message->suscriptoresConfirmados);
            
            t_cache_buffer* buffer = serializar_cacheLocalizedPokemon(&cache_localizedPoke, localizedPoke->posiciones);
            addressFromMessageToCopy = buffer;
            administrative->length = buffer->size;
            free(cache_localizedPoke.pokeName);
            free(cache_localizedPoke.posiciones);
            break;
        }
        default:
            log_error(broker_custom_logger, "Codigo de operación desconocido. No se puede cachear el mensaje");
    }

    //Etapa 1: Buscar partición libre
    uint32_t partitionsSize;
    uint32_t acumFreeSpace = 0;
    t_holes* targetHole = algoritmo_particion_libre(addressFromMessageToCopy->size); //Me asegura que de ese hole puedo sacar la particion minima por config
    if(targetHole != NULL)
        acumFreeSpace += targetHole->length;
    while(targetHole == NULL || acumFreeSpace < addressFromMessageToCopy->size)
    {
        if(FRECUENCIA_COMPACTACION == -1)
        {
            pthread_mutex_lock(&s_partitions);
                partitionsSize = list_size(partitions);
            pthread_mutex_unlock(&s_partitions);
            if(partitionsSize == 0) 
            { 
                compactar(); 
                targetHole = algoritmo_particion_libre(addressFromMessageToCopy->size);

                if(targetHole != NULL)
                    acumFreeSpace += targetHole->length;

                continue;
            }
        }
        else
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
                targetHole = algoritmo_particion_libre(addressFromMessageToCopy->size);
                pthread_mutex_unlock(&s_counterToCompactacion);

                if(targetHole != NULL)
                    acumFreeSpace += targetHole->length;

                continue;
            }
            pthread_mutex_unlock(&s_counterToCompactacion);
        }

        //Etapa 3: Reemplazo y Consolidacion
        targetHole = algoritmo_reemplazo(addressFromMessageToCopy->size);
        consolidar();

        if(targetHole != NULL)
            acumFreeSpace += targetHole->length;


        if(acumFreeSpace >= addressFromMessageToCopy->size)
            targetHole = algoritmo_particion_libre(addressFromMessageToCopy->size);
    }
    
    administrative->startAddress = targetHole->pStart;

    //Guardamos la estructura administrativa
    pthread_mutex_lock(&s_metadatas);
        list_add(metadatas, (void*)administrative);
    pthread_mutex_unlock(&s_metadatas);

    //Cuando el targetPartition ya esté ubicado se meten los datos donde corresponda
    writeData(administrative, targetHole, addressFromMessageToCopy);
}

void writeData(cache_message* administrative, t_holes* targetHole, t_cache_buffer* bufferMessage)
{
    t_partition* partition = createPartition(targetHole->pStart, targetHole->length);
    partition->id = administrative->idMessage;

    pthread_mutex_lock(&s_partitions);
        list_add(partitions, (void*)partition);
        list_sort(partitions, (void*)mem_address_menor_a_mayor);
    pthread_mutex_unlock(&s_partitions);
    
    pthread_mutex_lock(&s_cache);
        memcpy(administrative->startAddress, bufferMessage->stream, bufferMessage->size);
        // free(bufferMessage);
    pthread_mutex_unlock(&s_cache);
    log_info(logger, "Se ha guardado el mensaje id %i en la cache. Posicion de inicio: %p", administrative->idMessage, administrative->startAddress);
}

t_holes* reemplazo_fifo(uint32_t bytes)
{   
    t_partition* targetFifoPartition = NULL;

    //Se crea una copia para ordenarla por fifo
    pthread_mutex_lock(&s_partitions);
        t_list* tempPartitionsList = list_duplicate(partitions);
    pthread_mutex_unlock(&s_partitions);
    uint32_t _fifo_menor_a_mayor (t_partition* partition1, t_partition* partition2) {
        return partition1->fifoPosition < partition2->fifoPosition;
    }
    list_sort(tempPartitionsList, (void*)_fifo_menor_a_mayor);

    //Se toma el numero de fifo de la primera particion despues del orden
    targetFifoPartition = (t_partition*)list_get(tempPartitionsList, 0);

    if(targetFifoPartition == NULL) {
        log_error(broker_custom_logger, "Target Fifo Partition es NULL");
        exit(1);
    }

    uint32_t fifoToDelete = targetFifoPartition->fifoPosition;

    // free(targetFifoPartition);

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
        void* addressToFindItsMetadata;
        for(uint32_t i = 0; i < list_size(partitions); i++)
        {
            currentPartition = list_get(partitions, i);
            if(currentPartition->fifoPosition == victim->fifoPosition)
            {
                addressToFindItsMetadata = currentPartition->pStart;
                list_remove(partitions, i);
                break;
            }
        }
        list_destroy(tempPartitionsList);
    pthread_mutex_unlock(&s_partitions);

    //Se elimina la metadata
    pthread_mutex_lock(&s_metadatas);
        cache_message* metadata;
        for(uint32_t i = 0; i < list_size(metadatas); i++)
        {
            metadata = (cache_message*)list_get(metadatas, i);
            if(metadata->startAddress == addressToFindItsMetadata)
            {
                list_remove(metadatas, i);
                break;
            }
        }
    pthread_mutex_unlock(&s_metadatas);
    pthread_mutex_lock(&s_holes);
        list_add(holes, (void*)newFreeSpace);
    pthread_mutex_unlock(&s_holes);
    return newFreeSpace;
}

t_holes* reemplazo_lru(uint32_t bytes)
{ 
    t_partition* targetLruPartition = NULL;

    //Se crea una copia para ordenarla por fifo
    pthread_mutex_lock(&s_partitions);
        t_list* tempPartitionsList = list_duplicate(partitions);
    pthread_mutex_unlock(&s_partitions);
    uint32_t _lru_menor_a_mayor (t_partition* partition1, t_partition* partition2) {
        return partition1->lastUse < partition2->lastUse;
    }
    list_sort(tempPartitionsList, (void*)_lru_menor_a_mayor);

    //Se toma el numero de fifo de la primera particion despues del orden
    targetLruPartition = (t_partition*)list_get(tempPartitionsList, 0);

    if(targetLruPartition == NULL) {
        log_error(broker_custom_logger, "Target Fifo Partition es NULL");
        exit(1);
    }

    uint32_t fifoToDelete = targetLruPartition->fifoPosition;

    // free(targetLruPartition);

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
        void* addressToFindItsMetadata;
        for(uint32_t i = 0; i < list_size(partitions); i++)
        {
            currentPartition = list_get(partitions, i);
            if(currentPartition->fifoPosition == victim->fifoPosition)
            {
                addressToFindItsMetadata = currentPartition->pStart;
                list_remove(partitions, i);
                break;
            }
        }
        list_destroy(tempPartitionsList);
    pthread_mutex_unlock(&s_partitions);

    //Se elimina la metadata
    pthread_mutex_lock(&s_metadatas);
        cache_message* metadata;
        for(uint32_t i = 0; i < list_size(metadatas); i++)
        {
            metadata = (cache_message*)list_get(metadatas, i);
            if(metadata->startAddress == addressToFindItsMetadata)
            {
                list_remove(metadatas, i);
                break;
            }
        }
    pthread_mutex_unlock(&s_metadatas);
    pthread_mutex_lock(&s_holes);
        list_add(holes, (void*)newFreeSpace);
    pthread_mutex_unlock(&s_holes);
    return newFreeSpace;
}

void reallocPartition(t_partition* previousPartition, t_partition* newAllocatedPartition)
{
    uint32_t _is_the_metadata(cache_message* message) {
        return message->idMessage == previousPartition->id;
    }
    
    pthread_mutex_lock(&s_metadatas);
        cache_message* metadata = (cache_message*)list_find(metadatas, (void*)_is_the_metadata);
    pthread_mutex_unlock(&s_metadatas);
    if(metadata == NULL) {
        log_error(broker_custom_logger, "No se encontro la metadata para la particion %i", previousPartition->id);
        exit(1);
    }

    //Primero asignamos la metadata clave de la particion
    newAllocatedPartition->id = previousPartition->id;
    newAllocatedPartition->fifoPosition = previousPartition->fifoPosition;
    newAllocatedPartition->lastUse = previousPartition->lastUse;

    //Tomamos el mensaje para reubicarlo
    t_cache_buffer* buffer = malloc(sizeof(t_cache_buffer));
    buffer->stream = malloc(metadata->length);
    buffer->size = metadata->length;
    memcpy(buffer->stream, metadata->startAddress, metadata->length);

    //Reubicamos a la nueva posicion
    memcpy(newAllocatedPartition->pStart, buffer->stream, buffer->size);

    //Actualizamos la metadata de la particion
    metadata->startAddress = newAllocatedPartition->pStart;   
}

void dispatchCachedMessages(t_cache_dispatch_info* dispatchInfo)
{
    //Para filtrar los mensajes que pertenecen a la cola solicitada
    uint32_t _belongs_to_queue(cache_message* message) {
        return message->mq_cod == dispatchInfo->mq_cod;
    }

    //Chequea que no se le haya enviado antes el mensaje
    uint32_t _he_gots_the_message(t_suscripcion* suscription) {
        return suscription->idModule == dispatchInfo->suscripcion->idModule;
    }

    t_list* tempMetadatas = list_create(); //Lista de cache_message

    pthread_mutex_lock(&s_partitions);
    pthread_mutex_lock(&s_holes);
    pthread_mutex_lock(&s_metadatas);

    tempMetadatas = list_filter(metadatas, (void*)_belongs_to_queue);

    cache_message* currentMetadata;
    for(uint32_t i = 0; i < list_size(tempMetadatas); i++)
    {
        currentMetadata = list_get(tempMetadatas, i);

        if(list_size(currentMetadata->suscriptoresEnviados) > 0)
        {
            bool heGotTheMessage = list_any_satisfy(currentMetadata->suscriptoresEnviados, (void*)_he_gots_the_message);
            bool heAlreadyConfirmed = list_any_satisfy(currentMetadata->suscriptoresConfirmados, (void*)_he_gots_the_message);
            log_warning(broker_custom_logger, "%i : %i", heGotTheMessage, heAlreadyConfirmed);
            if(heGotTheMessage)
            {
                if(heAlreadyConfirmed)
                {
                    continue;
                }
            }
        }

        pthread_mutex_lock(&s_cache);
            t_cache_buffer* buffer = malloc(sizeof(t_cache_buffer));
            buffer->stream = malloc(currentMetadata->length);
            memcpy(buffer->stream, currentMetadata->startAddress, currentMetadata->length);
            
            t_message* message = crearMessage();
            message->id = currentMetadata->idMessage;
            message->idCorrelativo = currentMetadata->idCorrelational;
            message->mq_cod = currentMetadata->mq_cod;
            list_add_all(message->suscriptoresConfirmados, currentMetadata->suscriptoresConfirmados);
            list_add_all(message->suscriptoresEnviados, currentMetadata->suscriptoresEnviados);

            switch (dispatchInfo->mq_cod)
            {
                case NEW_POKEMON:
                {
                    cache_new_pokemon* cache_newPoke = deserializar_cacheNewPokemon(buffer);
                    t_new_pokemon* newPoke = newPokemon_cacheToMessage(cache_newPoke, currentMetadata);
                    message->mensaje = newPoke;
                    break;
                }
                case APPEARED_POKEMON:
                {
                    cache_appeared_pokemon* cache_appearedPoke = deserializar_cacheAppearedPokemon(buffer);
                    t_appeared_pokemon* appearedPoke = appearedPokemon_cacheToMessage(cache_appearedPoke, currentMetadata);
                    message->mensaje = appearedPoke;
                    break;
                }
                case CATCH_POKEMON:
                {
                    cache_catch_pokemon* cache_catchPoke = deserializar_cacheCatchPokemon(buffer);
                    t_catch_pokemon* catchPoke = catchPokemon_cacheToMessage(cache_catchPoke, currentMetadata);                    
                    message->mensaje = catchPoke;
                    break;
                }
                case CAUGHT_POKEMON:
                {
                    cache_caught_pokemon* cache_caughtPoke = deserializar_cacheCaughtPokemon(buffer);
                    t_caught_pokemon* caughtPoke = caughtPokemon_cacheToMessage(cache_caughtPoke, currentMetadata);                    
                    message->mensaje = caughtPoke;
                    break;
                }
                case GET_POKEMON:
                {
                    cache_get_pokemon* cache_getPokemon = deserializar_cacheGetPokemon(buffer);
                    t_get_pokemon* getPoke = getPokemon_cacheToMessage(cache_getPokemon, currentMetadata);
                    message->mensaje = getPoke;
                    break;
                }
                case LOCALIZED_POKEMON:
                {
                    cache_localized_pokemon* cache_localizedPokemon = deserializar_cacheLocalizedPokemon(buffer);
                    t_localized_pokemon* localizedPoke = localizedPokemon_cacheToMessage(cache_localizedPokemon, currentMetadata);
                    message->mensaje = localizedPoke;
                    break;
                }
                default:
                    log_error(broker_custom_logger, "Codigo de operación desconocido. No se puede cachear el mensaje");
            }

        pthread_mutex_unlock(&s_cache);
        updatePartitionLRU_byDataAddress(currentMetadata->startAddress);
        sendMessageFromQueue(message, dispatchInfo->suscripcion, currentMetadata);
    }
    pthread_mutex_unlock(&s_metadatas);
    pthread_mutex_unlock(&s_holes);
    pthread_mutex_unlock(&s_partitions);
    list_destroy(tempMetadatas);
}

void dump()
{
    FILE* fp;
    fp = fopen("./dumps/cache_dump.txt", "a");

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

    fprintf(fp, "\nDump: %02d/%02d/%d %02d:%02d:%02d\n",
        tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
    t_partition* currentBlock;
    cache_message* currentMetadata;
    
    pthread_mutex_lock(&s_metadatas);
    for (uint32_t i = 0; i < list_size(allMemory); i++)
    {
        currentBlock = (t_partition*)list_get(allMemory, i);
        uint32_t _is_the_metadata(cache_message* message) {
            return message->idMessage == currentBlock->id;
        }

        //Si es una particion ocupada
        if(currentBlock->free == 'X')
        {
            //Es igual pero le pone un espacio para acomodar columnas
            if(currentBlock->length < 10)
            {
                currentMetadata = (cache_message*)list_find(metadatas, (void*)_is_the_metadata);
                fprintf(fp, "Particion %i:  %p - %p    [%c]    Size: %ib     LRU: %llu    COLA: %i   ID: %i\n", 
                    i+1, currentBlock->pStart, currentBlock->pLimit, currentBlock->free, currentBlock->length, currentBlock->lastUse
                    , currentMetadata->mq_cod, currentMetadata->idMessage);
            }
            else
            {
                currentMetadata = (cache_message*)list_find(metadatas, (void*)_is_the_metadata);
                fprintf(fp, "Particion %i:  %p - %p    [%c]    Size: %ib    LRU: %llu    COLA: %i   ID: %i\n", 
                    i+1, currentBlock->pStart, currentBlock->pLimit, currentBlock->free, currentBlock->length, currentBlock->lastUse
                    , currentMetadata->mq_cod, currentMetadata->idMessage);
            }
        }
        else
        {
            fprintf(fp, "Particion %i:  %p - %p    [%c]    Size: %ib    LRU: -    COLA: -   ID: -\n", 
                i+1, currentBlock->pStart, currentBlock->pLimit, currentBlock->free, currentBlock->length);
        }
    }
    fclose(fp);
    pthread_mutex_unlock(&s_metadatas);
    printf("\n");
    log_info(logger, "Se ha solicitado un dump de la memoria cache");
}

void dumpConsole(char* titulo)
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

    printf("\n%s - \nDump: %02d/%02d/%d %02d:%02d:%02d\n", titulo,
        tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
    t_partition* currentBlock;
    cache_message* currentMetadata;
    
    pthread_mutex_lock(&s_metadatas);
    for (uint32_t i = 0; i < list_size(allMemory); i++)
    {
        currentBlock = (t_partition*)list_get(allMemory, i);
        uint32_t _is_the_metadata(cache_message* message) {
            return message->idMessage == currentBlock->id;
        }

        //Si es una particion ocupada
        if(currentBlock->free == 'X')
        {
            //Es igual pero le pone un espacio para acomodar columnas
            if(currentBlock->length < 10)
            {
                currentMetadata = (cache_message*)list_find(metadatas, (void*)_is_the_metadata);
                printf("Particion %i:  %p - %p    [%c]    Size: %ib     LRU: %llu    COLA: %i   ID: %i\n", 
                    i+1, currentBlock->pStart, currentBlock->pLimit, currentBlock->free, currentBlock->length, currentBlock->lastUse
                    , currentMetadata->mq_cod, currentMetadata->idMessage);
            }
            else
            {
                currentMetadata = (cache_message*)list_find(metadatas, (void*)_is_the_metadata);
                printf("Particion %i:  %p - %p    [%c]    Size: %ib    LRU: %llu    COLA: %i   ID: %i\n", 
                    i+1, currentBlock->pStart, currentBlock->pLimit, currentBlock->free, currentBlock->length, currentBlock->lastUse
                    , currentMetadata->mq_cod, currentMetadata->idMessage);
            }
        }
        else
        {
            printf("Particion %i:  %p - %p    [%c]    Size: %ib    LRU: -    COLA: -   ID: -\n", 
                i+1, currentBlock->pStart, currentBlock->pLimit, currentBlock->free, currentBlock->length);
        }
    }
    pthread_mutex_unlock(&s_metadatas);
    printf("\n");
}

void consolidar()
{
    pthread_mutex_lock(&s_holes);
    if(list_size(holes) == 0 || list_size(holes) == 1)
    {
        pthread_mutex_unlock(&s_holes);
        return;
    }
    list_sort(holes, (void*) mem_address_menor_a_mayor);
    pthread_mutex_unlock(&s_holes);
    t_holes* lowerHole;
    t_holes* upperHole;

    uint32_t i = 0;
    pthread_mutex_lock(&s_holes);
    while (list_size(holes) >= 2 && i < list_size(holes) - 1)
    {        
        lowerHole = list_get(holes, i);
        upperHole = list_get(holes, i + 1);

        if(lowerHole->pLimit == upperHole->pStart - 1)
        {
            //Inicia la Consolidacion
            t_holes* consolidado = createHole(lowerHole->pStart, lowerHole->length + upperHole->length);

            //Se elimina la posición i dos veces porque el espacio contiguo se desplaza un lugar
            list_remove(holes, i);
            list_remove(holes, i);
            list_add(holes, consolidado);

            //Ordenamos para siempre comparar los primeros 2 holes
            list_sort(holes, (void*) mem_address_menor_a_mayor);
            i = 0;
            continue;
        }
        i++;
    }
    pthread_mutex_unlock(&s_holes);
}

void compactar()
{
    pthread_mutex_lock(&s_holes);
    pthread_mutex_lock(&s_partitions);
    if(list_size(partitions) == 0 || list_size(holes) == 0)
    {
        log_info(broker_custom_logger, "No hay particiones o no hay huecos -> No es necesario compactar");
        return;
    }
    pthread_mutex_unlock(&s_holes);
    pthread_mutex_unlock(&s_partitions);

    //Ordenar ambas listas
    list_sort(partitions, (void*) mem_address_menor_a_mayor);
    list_sort(holes, (void*) mem_address_menor_a_mayor);
    uint32_t targetPartitionIndex;
    t_partition* targetPartition;

    consolidar(); //El algoritmo requiere que los huecos esten consolidados antes de compactar
    while (existHolesBetweenPartitions())
    {
        pthread_mutex_lock(&s_holes);
            t_holes* hole = (t_holes*)list_get(holes, 0);
        pthread_mutex_unlock(&s_holes);
        //Busco la particion contigua al hueco
        targetPartitionIndex = -1;
        pthread_mutex_lock(&s_partitions);
        for(uint32_t i = 0; i < list_size(partitions); i++)
        {
            targetPartition = list_get(partitions, i);
            if(targetPartition->pStart == hole->pLimit + 1)
            {
                targetPartitionIndex = i;
                break;
            }
        }
        pthread_mutex_unlock(&s_partitions);

        if(targetPartitionIndex == -1)
            log_error(broker_custom_logger, "No hay particiones contiguas al ultimo hueco");

        //Se hace un swap entre la particion y el hueco
        t_partition* relocatedPartition = createPartition(hole->pStart, targetPartition->length);
        t_holes* relocatedHole = createHole(relocatedPartition->pLimit + 1, hole->length);

        //Hay que actualizar las nuevas direcciones de la particion
        reallocPartition(targetPartition, relocatedPartition);

        pthread_mutex_lock(&s_holes);
            list_remove(holes, 0);
            list_add(holes, relocatedHole);
            list_sort(holes, (void*) mem_address_menor_a_mayor);
        pthread_mutex_unlock(&s_holes);
        pthread_mutex_lock(&s_partitions);
            list_remove(partitions, targetPartitionIndex);
            list_add(partitions, relocatedPartition);
            list_sort(partitions, (void*) mem_address_menor_a_mayor);
        pthread_mutex_unlock(&s_partitions);

        targetPartition = NULL;

        consolidar(); //El algoritmo requiere que los huecos se vayan consolidando a medida que se mueven las particiones
    }
    consolidar(); //Consolidacion post compactacion
    log_info(logger, "Se ha realizado una compactacion");
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
                t_holes* removedHole = list_remove(holes, i);
                freeHole(removedHole);
                break;
            }
            else if (currentHole->length == bytesToAlloc)
            {
                result = createHole(currentHole->pStart, bytesToAlloc);
                t_holes* removedHole = list_remove(holes, i);
                freeHole(removedHole);
                break;
            }
        }
    pthread_mutex_unlock(&s_holes);
    
    pthread_mutex_lock(&s_holes);
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

    t_holes* result = NULL;
    t_holes* currentHole = NULL;
    t_holes* bestHole = NULL;
    uint32_t indexOfBestHole = -1;
    pthread_mutex_lock(&s_holes);
    for(int i = 0; i < list_size(holes); i++)
    {
        currentHole = list_get(holes, i);
        if(bestHole == NULL && currentHole->length >= bytesToAlloc)
        {
            bestHole = currentHole;
            indexOfBestHole = i;
        }
        else if (currentHole->length >= bytesToAlloc && currentHole->length < bestHole->length)
        {
            bestHole = currentHole;
            indexOfBestHole = i;
            if(bestHole->length == bytesToAlloc) //Si justo tiene el tamaño justo no hay que buscar mas
                break;
        } 
    }
    pthread_mutex_unlock(&s_holes);

    if(bestHole == NULL)
        return NULL;

    if(bestHole->length == bytesToAlloc) //No hace falta dividir el hueco
    {
        result = createHole(currentHole->pStart, bytesToAlloc);
        pthread_mutex_lock(&s_holes);
            list_remove(holes, indexOfBestHole);
        pthread_mutex_unlock(&s_holes);
    }
    else
    {
        //Hacer el split
        t_holes* splittedHole = createHole(currentHole->pStart + bytesToAlloc, (currentHole->length - bytesToAlloc));
        pthread_mutex_lock(&s_holes);
            list_add(holes, (void*)splittedHole);
            list_remove(holes, indexOfBestHole);
            list_sort(holes, (void*)mem_address_menor_a_mayor);
        pthread_mutex_unlock(&s_holes);
        result = createHole(currentHole->pStart, bytesToAlloc);
    }    
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

void updatePartitionLRU_byDataAddress(void* startAddress)
{
    //Cuando se llama a esta funcion, ya esta adentro de un wait, por eso no hay mutex aca.
    uint32_t is_the_partition_with_that_address(t_partition* partition) {
        return partition->pStart == startAddress;
    }
    t_partition* targetPartition = list_find(partitions, (void*)is_the_partition_with_that_address);
    if (targetPartition != NULL) {
        targetPartition->lastUse = getTimestamp();
    }
}

uint32_t mem_address_menor_a_mayor(t_holes* block, t_holes* block2)
{
    return block->pStart < block2->pStart;
}

uint32_t existHolesBetweenPartitions()
{
    pthread_mutex_lock(&s_holes);
    pthread_mutex_lock(&s_partitions);
        t_holes* firstHole = list_get(holes, 0); //El primer hueco
        t_partition* lastPartition = list_get(partitions, list_size(partitions) - 1); //La ultima particion
    pthread_mutex_unlock(&s_holes);
    pthread_mutex_unlock(&s_partitions);
    return firstHole->pStart < lastPartition->pLimit;
}

uint64_t getTimestamp()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return 1000000 * tv.tv_sec + tv.tv_usec;
}


void freeCacheMessage(cache_message* cacheMessage)
{
    list_destroy(cacheMessage->suscriptoresConfirmados);
    list_destroy(cacheMessage->suscriptoresEnviados);
    free(cacheMessage);
}

void freeHole(t_holes* hole)
{
    free(hole);
}

void freePartition(t_partition* partition)
{
    free(partition);
}

void freeCacheSystem()
{
    list_destroy_and_destroy_elements(partitions, free);
    list_destroy_and_destroy_elements(holes, (void*)freeHole);
    list_destroy_and_destroy_elements(metadatas, (void*)freeCacheMessage);
    free(cache);
}