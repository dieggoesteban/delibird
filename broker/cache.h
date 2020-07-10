#ifndef CACHE_H_
#define CACHE_H_

#include "global-includes.h"
#include "models.h"

void* cache;

t_list* partitions;
t_list* holes;

pthread_mutex_t* s_counterToCompactacion;
uint32_t counterToCompactacion;

typedef struct
{
    void* pStart;
    void* pLimit;
    uint32_t length;
} t_partition;

typedef struct
{
    void* pStart;
    void* pLimit;
    uint32_t length; 
} t_holes;

uint32_t TAMANO_MEMORIA;
uint32_t TAMANO_MINIMO_PARTICION;
char* ALGORITMO_MEMORIA;
char* ALGORITMO_REEMPLAZO;
char* ALGORITMO_PARTICION_LIBRE;
uint32_t FRECUENCIA_COMPACTACION;

void (*algoritmo_memoria) ();
void (*algoritmo_reemplazo) ();
void* (*algoritmo_particion_libre) ();

void startCache();
void memoria_buddySystem();
void memoria_particiones();

void reemplazo_fifo();
void reemplazo_bf();

void* particionLibre_ff(uint32_t bytes);
void* particionLibre_bf(uint32_t bytes);

void compactar();
void consolidar();

void cacheMessage(t_message* message);
void dispatchCachedMessages(uint32_t subscriber);

//AUX Methods for dev purposes
t_holes* createHole(void* startAddress, uint32_t length);
void showHoles();
bool mem_address_menor_a_mayor(t_holes* hole1, t_holes* hole2);
#endif /* CACHE_H_ */