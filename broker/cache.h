#ifndef CACHE_H_
#define CACHE_H_

#include "global-includes.h"
#include "models.h"
#include <signal.h>
#include <time.h>

void* cache;

t_list* partitions;
t_list* holes;
t_list* administrative;

pthread_mutex_t s_counterToCompactacion;
pthread_mutex_t s_holes;
pthread_mutex_t s_partitions;

uint32_t counterToCompactacion;

typedef struct
{
    void* pStart;
    void* pLimit;
    uint32_t length;
    char free;
} t_partition;

typedef struct
{
    void* pStart;
    void* pLimit;
    uint32_t length; 
    char free;
} t_holes;

uint32_t TAMANO_MEMORIA;
uint32_t TAMANO_MINIMO_PARTICION;
char* ALGORITMO_MEMORIA;
char* ALGORITMO_REEMPLAZO;
char* ALGORITMO_PARTICION_LIBRE;
uint32_t FRECUENCIA_COMPACTACION;

void (*algoritmo_memoria) (t_message*);
void (*algoritmo_reemplazo) ();
t_holes* (*algoritmo_particion_libre) (uint32_t bytes);

void startCache();
void memoria_buddySystem();
void memoria_particiones();

void reemplazo_fifo();
void reemplazo_lru();

t_holes* particionLibre_ff(uint32_t bytes);
t_holes* particionLibre_bf(uint32_t bytes);

void compactar();
void consolidar();

void cacheMessage(t_message* message);
void dispatchCachedMessages(t_suscripcion* subscriber);
void dump();
//AUX Methods for dev purposes
void writeData(cache_message* administrative, t_holes* targetHole);
t_holes* createHole(void* startAddress, uint32_t length);
void showHoles();
t_partition* createPartition(void* startAddress, uint32_t length);
void showPartitions();
uint32_t mem_address_menor_a_mayor(t_holes* hole1, t_holes* hole2);
uint32_t existHolesBetweenPartitions();


//Creacion y serializacion de mensajes con estructura de cache
cache_message* createCacheMessage(t_message* message);

#endif /* CACHE_H_ */