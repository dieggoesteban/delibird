#ifndef CACHE_H_
#define CACHE_H_

#include "global-includes.h"
#include "models.h"
#include "cacheSerializador.h"
#include "messageQueueManager.h"
#include <signal.h>
#include <sys/time.h>

void* cache;

t_list* partitions;
t_list* holes;
t_list* metadatas;

pthread_mutex_t s_cache;
pthread_mutex_t s_counterToCompactacion;
pthread_mutex_t s_partitionCounter;
pthread_mutex_t s_partitions;
pthread_mutex_t s_holes;
pthread_mutex_t s_metadatas;

uint32_t partitionCounter;
uint32_t counterToCompactacion;

typedef struct
{
    uint32_t id;
    uint32_t fifoPosition;
    void* pStart;
    void* pLimit;
    uint32_t length;
    char free;
    uint64_t lastUse;
} t_partition;

typedef struct
{
    uint32_t id;
    uint32_t fifoPosition;
    void* pStart;
    void* pLimit;
    uint32_t length; 
    char free;
    uint64_t lastUse;
} t_holes;


uint32_t TAMANO_MEMORIA;
uint32_t TAMANO_MINIMO_PARTICION;
char* ALGORITMO_MEMORIA;
char* ALGORITMO_REEMPLAZO;
char* ALGORITMO_PARTICION_LIBRE;
uint32_t FRECUENCIA_COMPACTACION;

void (*algoritmo_memoria) (t_message*);
t_holes* (*algoritmo_reemplazo) (uint32_t bytes);
t_holes* (*algoritmo_particion_libre) (uint32_t bytes);

void startCache(); 
void memoria_buddySystem();
void memoria_particiones();

t_holes* reemplazo_fifo(uint32_t bytes);
t_holes* reemplazo_lru(uint32_t bytes);

t_holes* particionLibre_ff(uint32_t bytes);
t_holes* particionLibre_bf(uint32_t bytes);

void compactar();
void consolidar();

void cacheMessage(t_message* message);
void reallocPartition(t_partition* previousPartition, t_partition* newAllocatedPartition);
void dispatchCachedMessages(t_cache_dispatch_info* dispatchInfo);
void dump();
void dumpConsole(char* titulo);
void writeData(cache_message* administrative, t_holes* targetHole, t_cache_buffer* bufferMessage);

t_holes* createHole(void* startAddress, uint32_t length);
t_partition* createPartition(void* startAddress, uint32_t length);

void showHoles();
void showPartitions();
void updatePartitionLRU_byDataAddress(void* startAddress);
uint32_t mem_address_menor_a_mayor(t_holes* hole1, t_holes* hole2);
uint32_t existHolesBetweenPartitions();

uint64_t getTimestamp();

void freePartition(t_partition* partition);
void freeHole(t_holes* hole);
void freeCacheMessage(cache_message* cacheMessage);
void freeCacheSystem();
#endif /* CACHE_H_ */