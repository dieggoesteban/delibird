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
    partitions = list_create();
    holes = list_create();

    if(pthread_mutex_init(&s_counterToCompactacion, NULL) != 0)
		log_error(broker_custom_logger, "Error mutex init (s_counterToCompactacion) in cache initialization");
    if(pthread_mutex_init(&s_holes, NULL) != 0)
		log_error(broker_custom_logger, "Error mutex init (s_holes) in cache initialization");
    if(pthread_mutex_init(&s_partitions, NULL) != 0)
	    log_error(broker_custom_logger, "Error mutex init (s_partitions) in cache initialization");
    
    //Memory allocation
    cache = malloc(TAMANO_MEMORIA);
    t_holes* initial = createHole(cache, TAMANO_MEMORIA);
    list_add(holes, initial);

    //log_info(broker_custom_logger, "Cache inicializada correctamente");

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
            memcpy(&cache_newPoke.pokeName, newPoke->nombre, newPoke->sizeNombre);
            cache_newPoke.cantidad = newPoke->posicionCantidad->cantidad;
            cache_newPoke.posX = newPoke->posicionCantidad->posicion_x;
            cache_newPoke.posY = newPoke->posicionCantidad->posicion_y;
            
            administrative->idMessage = newPoke->ID_mensaje_recibido;
            administrative->mq_cod = NEW_POKEMON;
            administrative->length = (sizeof(uint32_t) * 4 + (cache_newPoke.nameLength - 1));
            administrative->suscriptoresEnviados = list_duplicate(message->suscriptoresEnviados);
            administrative->suscriptoresConfirmados = list_duplicate(message->suscriptoresConfirmados);
            
            addressFromMessageToCopy = &newPoke;
            sizeOfMessage = administrative->length;
            break;
        }
        case APPEARED_POKEMON:
        {
            t_appeared_pokemon* appearedPoke = (t_appeared_pokemon*)message->mensaje;
            cache_appeared_pokemon cache_appeared;
            cache_appeared.nameLength = appearedPoke->sizeNombre;            
            strcpy(cache_appeared.pokeName, appearedPoke->nombre);
            cache_appeared.posX = appearedPoke->posicion->posicion_x;
            cache_appeared.posY = appearedPoke->posicion->posicion_y;
            
            administrative->idMessage = appearedPoke->ID_mensaje_recibido;
            administrative->mq_cod = APPEARED_POKEMON;
            administrative->length = sizeof(cache_appeared);
            administrative->suscriptoresEnviados = list_duplicate(message->suscriptoresEnviados);
            administrative->suscriptoresConfirmados = list_duplicate(message->suscriptoresConfirmados);
            break;
        }
        case CATCH_POKEMON:
            break;
        case CAUGHT_POKEMON:
            break;
        case GET_POKEMON:
            break;
        case LOCALIZED_POKEMON: 
            break;
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
            pthread_mutex_lock(&s_holes);
            pthread_mutex_lock(&s_partitions);
                compactar();
            pthread_mutex_unlock(&s_partitions);
            pthread_mutex_unlock(&s_holes);

            counterToCompactacion = 0;
            targetHole = algoritmo_particion_libre(sizeOfMessage);
            continue;
        }
        pthread_mutex_unlock(&s_counterToCompactacion);
        
        //Etapa 3: Reemplazo y Consolidacion
        algoritmo_reemplazo();
        pthread_mutex_lock(&s_holes);
            consolidar();
        pthread_mutex_unlock(&s_holes);

        targetHole = algoritmo_particion_libre(sizeOfMessage);
    }
    
    administrative->startAddress = targetHole->pStart;

    //Cuando el targetPartition ya esté ubicado se meten los datos donde corresponda
    writeData(administrative, targetHole, addressFromMessageToCopy);
}

void writeData(cache_message* administrative, t_holes* targetHole, void* message)
{
    //Add mutex
    t_partition* partition = createPartition(targetHole->pStart, targetHole->length);
    list_add(partitions, (void*)partition);
    list_sort(partitions, (void*)mem_address_menor_a_mayor);
    
    memcpy(administrative->startAddress, message, administrative->length);
}

void reemplazo_fifo(){}
void reemplazo_lru(){}

void dispatchCachedMessages(t_suscripcion* subscriber){}

void dump()
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    //Añadir locks
    t_list* allMemory = list_create();
    list_add_all(allMemory, partitions);
    list_add_all(allMemory, holes);

    list_sort(allMemory, (void*)mem_address_menor_a_mayor);

    printf("\nDump: %02d/%02d/%d %02d:%02d:%02d\n",
        tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
    t_partition* currentBlock;
    for (uint32_t i = 0; i < list_size(allMemory); i++)
    {
        currentBlock = (t_partition*)list_get(allMemory, i);
        printf("Partition %i:  %p - %p    [%c]    Size: %ib    LRU:    COLA:    ID:\n", 
            i+1, currentBlock->pStart, currentBlock->pLimit, currentBlock->free, currentBlock->length);
    }
}


void consolidar()
{
    if(list_size(holes) == 0 || list_size(holes) == 1)
        return;

    t_holes* lowerHole = (t_holes*)malloc(sizeof(t_holes));
    t_holes* upperHole = (t_holes*)malloc(sizeof(t_holes));

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
    //TODO: Donde hago el free? Si no me rompe la lista
    // free(lowerHole);
    // free(upperHole);
}
void compactar()
{
    if(list_size(partitions) == 0 || list_size(holes) == 0)
    {
        log_warning(broker_custom_logger, "No hay particiones o no hay huecos");
        return;
    }

    //Primero asegurar que no haya huecos contiguos y luego ordenar ambas listas
    consolidar();
    list_sort(partitions, (void*) mem_address_menor_a_mayor);
    list_sort(holes, (void*) mem_address_menor_a_mayor);
    uint32_t targetPartitionIndex;
    t_partition* targetPartition = (t_partition*)malloc(sizeof(t_partition));

    while (existHolesBetweenPartitions())
    {
        showPartitions();
        showHoles();
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
                list_remove(holes, i);
                list_sort(holes, (void*)mem_address_menor_a_mayor);
                break;
            }
            else if (currentHole->length == bytesToAlloc)
            {
                result = createHole(currentHole->pStart, bytesToAlloc);
                list_remove(holes, i);
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
    return hole;
}
void showHoles()
{
    t_holes* currentHole;

    pthread_mutex_lock(&s_holes);
    for(uint32_t i = 0; i < list_size(holes); i++)
    {
        currentHole = list_get(holes, i);
        printf("Hole %d - Range: %lu / %lu - Length: %d\n", i, 
            (unsigned long)currentHole->pStart%1000000, (unsigned long)currentHole->pLimit%1000000, currentHole->length);
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
    return partition;
}
void showPartitions()
{
    t_partition* currentPartition;

    pthread_mutex_lock(&s_partitions);
    for(uint32_t i = 0; i < list_size(partitions); i++)
    {
        currentPartition = list_get(partitions, i);
        printf("Partition %d - Range: %lu / %lu - Length: %d\n", i, 
            (unsigned long)currentPartition->pStart%1000000, (unsigned long)currentPartition->pLimit%1000000, currentPartition->length);
    }
    pthread_mutex_unlock(&s_partitions);
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
    pthread_mutex_unlock(&s_partitions);
    pthread_mutex_unlock(&s_holes);

    return firstHole->pStart < lastPartition->pLimit;
}