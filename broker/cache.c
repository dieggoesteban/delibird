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
        algoritmo_reemplazo = reemplazo_bf;

    if(strcmp(ALGORITMO_PARTICION_LIBRE, "FF") == 0)
        algoritmo_particion_libre = particionLibre_ff;
    else if(strcmp(ALGORITMO_PARTICION_LIBRE, "BF") == 0)
        algoritmo_particion_libre = particionLibre_bf;

    #pragma endregion

    //Initial params
    counterToCompactacion = 0;
    partitions = list_create();
    holes = list_create();

    s_counterToCompactacion = malloc(sizeof(pthread_mutex_t));
    s_holes = malloc(sizeof(pthread_mutex_t));
    s_partitions = malloc(sizeof(pthread_mutex_t));
    if(pthread_mutex_init(s_counterToCompactacion, NULL) != 0)
		log_error(broker_custom_logger, "Error mutex init (s_counterToCompactacion) in cache initialization");
    if(pthread_mutex_init(s_holes, NULL) != 0)
		log_error(broker_custom_logger, "Error mutex init (s_holes) in cache initialization");
    if(pthread_mutex_init(s_partitions, NULL) != 0)
		log_error(broker_custom_logger, "Error mutex init (s_partitions) in cache initialization");


    //Memory allocation
    cache = malloc(TAMANO_MEMORIA);
    // t_holes* initial = (t_holes*)malloc(sizeof(t_holes));
    // initial->pStart = cache;
    // initial->length = TAMANO_MEMORIA;
    // list_add(holes, initial);

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
    /*
    #pragma region Prueba de compactacion
    
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
        showHoles();
    #pragma endregion
    */
}

void memoria_buddySystem(t_message* message) {
    log_info(broker_custom_logger, "I'm using Buddy System Algorithm\n");

    //not implemented
}

void memoria_particiones(t_message* message) {
    log_info(broker_custom_logger, "Dynamic Partitions Algorithm\n");

    //Etapa 1: Buscar partición libre
    t_partition* targetPartition = (t_partition*)malloc(sizeof(t_partition));
    targetPartition = algoritmo_particion_libre(4);

    while(targetPartition == NULL)
    {
        pthread_mutex_lock(s_counterToCompactacion);
        if(counterToCompactacion < FRECUENCIA_COMPACTACION)
        {
            counterToCompactacion++;
        }
        else
        {
            pthread_mutex_lock(s_holes);
            pthread_mutex_lock(s_partitions);
                compactar();
            pthread_mutex_unlock(s_partitions);
            pthread_mutex_unlock(s_holes);

            counterToCompactacion = 0;
            targetPartition = algoritmo_particion_libre(4);
            continue;
        }
        pthread_mutex_unlock(s_counterToCompactacion);
        //Etapa 3: Reemplazo y Consolidacion
        algoritmo_reemplazo();

        pthread_mutex_lock(s_holes);
            consolidar();
        pthread_mutex_unlock(s_holes);
        targetPartition = algoritmo_particion_libre(4);
    }
    
    //Cuando el targetPartition ya esté ubicado se meten los datos donde corresponda
    
}

void reemplazo_fifo(){}
void reemplazo_bf(){}

void dispatchCachedMessages(uint32_t subscriber){}
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

void* particionLibre_ff(uint32_t bytes) 
{ 
    uint32_t bytesToAlloc = 0;
    if(bytes > TAMANO_MINIMO_PARTICION)
        bytesToAlloc = bytes;
    else
        bytesToAlloc = TAMANO_MINIMO_PARTICION;

    t_holes* currentPartition = (t_holes*)malloc(sizeof(t_holes));

    pthread_mutex_lock(s_holes);
        for(int i = 0; i < list_size(holes); i++)
        {
            currentPartition = list_get(holes, i);
            if (currentPartition->length >= bytesToAlloc)
            {
                return currentPartition->pStart;
            }
        }
    pthread_mutex_unlock(s_holes);

    free(currentPartition);
    return NULL;
}

void* particionLibre_bf(uint32_t bytes)
{
    uint32_t bytesToAlloc = 0;
    if(bytes > TAMANO_MINIMO_PARTICION)
        bytesToAlloc = bytes;
    else
        bytesToAlloc = TAMANO_MINIMO_PARTICION;

    void* result = NULL;
    t_holes* currentPartition = (t_holes*)malloc(sizeof(t_holes));
    t_holes* bestFit = (t_holes*)malloc(sizeof(t_holes));

    pthread_mutex_lock(s_holes);
        for(uint32_t i = 0; i < list_size(holes); i++)
        {
            currentPartition = list_get(holes, i);
            if(bestFit == NULL && currentPartition->length >= bytesToAlloc)
                bestFit = currentPartition;
            else if (currentPartition->length >= bytesToAlloc && currentPartition->length < bestFit->length)
            {
                bestFit = currentPartition;
                if(bestFit->length == bytesToAlloc)
                    break;
            }      
        }
    pthread_mutex_unlock(s_holes);

    result = bestFit->pStart;

    free(currentPartition);
    free(bestFit);
    return result;
}


//AUX Methods
t_holes* createHole(void* startAddress, uint32_t length)
{
    t_holes* hole = (t_holes*)malloc(sizeof(t_holes));
    hole->pStart = startAddress;
    hole->pLimit = startAddress + length - 1; //Se resta uno porque hay que incluir el byte de pStart
    hole->length = length; 
    return hole;
}
void showHoles()
{
    t_holes* currentHole = (t_holes*)malloc(sizeof(t_holes));

    pthread_mutex_lock(s_holes);
    for(uint32_t i = 0; i < list_size(holes); i++)
    {
        currentHole = list_get(holes, i);
        printf("Hole %d - Range: %lu / %lu - Length: %d\n", i, 
            (unsigned long)currentHole->pStart%1000000, (unsigned long)currentHole->pLimit%1000000, currentHole->length);
    }
    pthread_mutex_unlock(s_holes);
    //free(currentHole);
}

t_partition* createPartition(void* startAddress, uint32_t length)
{
    t_partition* partition = (t_partition*)malloc(sizeof(t_partition));
    partition->pStart = startAddress;
    partition->pLimit = startAddress + length - 1; //Se resta uno porque hay que incluir el byte de pStart
    partition->length = length; 
    return partition;
}
void showPartitions()
{
    t_partition* currentPartition = (t_partition*)malloc(sizeof(t_partition));

    pthread_mutex_lock(s_partitions);
    for(uint32_t i = 0; i < list_size(partitions); i++)
    {
        currentPartition = list_get(partitions, i);
        printf("Partition %d - Range: %lu / %lu - Length: %d\n", i, 
            (unsigned long)currentPartition->pStart%1000000, (unsigned long)currentPartition->pLimit%1000000, currentPartition->length);
    }
    pthread_mutex_unlock(s_partitions);
    //free(currentPartition);
}

uint32_t mem_address_menor_a_mayor(t_holes* hole1, t_holes* hole2)
{
    return hole1->pStart < hole2->pStart;
}

uint32_t existHolesBetweenPartitions()
{
    pthread_mutex_lock(s_holes);
    pthread_mutex_lock(s_partitions);
        t_holes* firstHole = list_get(holes, 0); //El primer hueco
        t_partition* lastPartition = list_get(partitions, list_size(partitions) - 1); //La ultima particion
    pthread_mutex_unlock(s_partitions);
    pthread_mutex_unlock(s_holes);

    return firstHole->pStart < lastPartition->pLimit;
}

cache_message* createCacheMessage(t_message* message)
{
    cache_message* cacheMessage = (cache_message*)malloc(sizeof(cache_message));
    cacheMessage->message = message;
    
    switch(message->mq_cod)
    {
        case NEW_POKEMON:
        {
            t_new_pokemon* o_newPokemon = (t_new_pokemon*)malloc(sizeof(t_new_pokemon)); //o: original
            cache_new_pokemon* c_newPokemon = (cache_new_pokemon*)malloc(sizeof(cache_new_pokemon)); //c: cached
            o_newPokemon = (t_new_pokemon*)message->mensaje;

            c_newPokemon->nameLength = o_newPokemon->sizeNombre;
            c_newPokemon->pokeName = o_newPokemon->nombre;
            c_newPokemon->cantidad = o_newPokemon->posicionCantidad->cantidad;
            c_newPokemon->posX = o_newPokemon->posicionCantidad->posicion_x;
            c_newPokemon->posY = o_newPokemon->posicionCantidad->posicion_y;
        }
    }

    return cacheMessage;
}