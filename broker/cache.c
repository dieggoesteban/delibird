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

    //Memory allocation
    cache = malloc(TAMANO_MEMORIA);
    // t_holes* initial = (t_holes*)malloc(sizeof(t_holes));
    // initial->pStart = cache;
    // initial->length = TAMANO_MEMORIA;
    // list_add(holes, initial);

    #pragma region Prueba de consolidacion

        t_holes* initialPart1 = createHole(cache, 1024);
        t_holes* initialPart2 = createHole(initialPart1->pLimit + 1, 1024);
        t_holes* initialPart3 = createHole(initialPart2->pLimit + 1, 512);
        t_holes* initialPart4 = createHole(initialPart3->pLimit + 12, 3096);

        list_add(holes, initialPart1);
        list_add(holes, initialPart2);
        list_add(holes, initialPart3);
        list_add(holes, initialPart4);
        showHoles();
        consolidar();
        printf("\n");
        showHoles();
        log_info(logger, "Cache module has started successfully\n");

        // free(initial);
        // free(cache);
        // free(initialPart1);
        // free(initialPart2);
    
    #pragma endregion
}

void memoria_buddySystem() {
    log_info(broker_custom_logger, "I'm using Buddy System Algorithm\n");

    //not implemented
}

void memoria_particiones() {
    log_info(broker_custom_logger, "I'm using Dynamic Partitions Algorithm\n");

    //Etapa 1: Buscar partición libre
    t_partition* targetPartition = (t_partition*)malloc(sizeof(t_partition));
    targetPartition = algoritmo_particion_libre();

    while(targetPartition == NULL)
    {
        if(counterToCompactacion < FRECUENCIA_COMPACTACION)
        {
            pthread_mutex_lock(s_counterToCompactacion);
            counterToCompactacion++;
            pthread_mutex_unlock(s_counterToCompactacion);
        }
        else
        {
            compactar();
            pthread_mutex_lock(s_counterToCompactacion);
            counterToCompactacion = 0;
            pthread_mutex_unlock(s_counterToCompactacion);
            targetPartition = algoritmo_particion_libre();
            continue;
        }

        //Etapa 3: Reemplazo y Consolidacion
        algoritmo_reemplazo();
        consolidar();

        targetPartition = algoritmo_particion_libre();
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
        return;

    
}

void* particionLibre_ff(uint32_t bytes) 
{ 
    //TODO: Agregar los semaforos
    t_holes* currentPartition = (t_holes*)malloc(sizeof(t_holes));
    for(int i = 0; i < list_size(holes); i++)
    {
        currentPartition = list_get(holes, i);
        if (currentPartition->length >= bytes)
        {
            return currentPartition->pStart;
        }
    }
    free(currentPartition);
    return NULL;
}

void* particionLibre_bf(uint32_t bytes)
{
    void* result = NULL;
    //TODO: Agregar los semaforos
    t_holes* currentPartition = (t_holes*)malloc(sizeof(t_holes));
    t_holes* bestFit = (t_holes*)malloc(sizeof(t_holes));
    for(uint32_t i = 0; i < list_size(holes); i++)
    {
        currentPartition = list_get(holes, i);
        if(bestFit == NULL && currentPartition->length >= bytes)
            bestFit = currentPartition;
        else if (currentPartition->length >= bytes && currentPartition->length < bestFit->length)
        {
            bestFit = currentPartition;
            if(bestFit->length == bytes)
                break;
        }      
    }

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
    for(uint32_t i = 0; i < list_size(holes); i++)
    {
        currentHole = list_get(holes, i);
        printf("Holes %d - Range: %p / %p - Length: %d\n", i, currentHole->pStart, currentHole->pLimit, currentHole->length);
    }
    //free(currentHole);
}
bool mem_address_menor_a_mayor(t_holes* hole1, t_holes* hole2)
{
    return hole1->pStart < hole2->pStart;
}