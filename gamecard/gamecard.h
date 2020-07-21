#ifndef GAMECARD_H_
#define GAMECARD_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include<commons/bitarray.h>
#include "semaphore.h"

#include "utils.h"
#include "sockets.h"
#include "models.h"
#include "tallgrass.h"


//GLOBALES
t_config* config;
t_log* logger;
char* IP;
char* PUERTO;
char* IP_BROKER;
char* PUERTO_BROKER;
uint32_t tiempoReintentoConexion;
uint32_t tiempoReintentoOperacion;
uint32_t tiempoOperacion;
char* punto_montaje;
char* pathFiles;
char* pathBlocks;
char* pathBitmap;
char* pathMetadata;
t_list* semaforosPokemon;
t_bitarray* bitmapArr;
uint32_t cantBloques;
uint32_t sizeBloque;
char* magicNumber;
uint32_t idModule;
t_suscribe* suscribeNew;
t_suscribe* suscribeCatch;
t_suscribe* suscribeGet;


//HILOS
pthread_t threadSERVER;
pthread_t threadSUSCRIBE_CATCH;
pthread_t threadSUSCRIBE_NEW;
pthread_t threadSUSCRIBE_GET;
pthread_t threadRECONNECT;
pthread_t threadDETECT_DISCON;
pthread_t finalizarPrograma;
pthread_mutex_t mutexBitmap;


//SEMAFOROS
sem_t semReconexion;
sem_t waitForFinish;
sem_t mutexReconnect;

//PROGRAMA
void inicializarGamecard();
void terminar_programa();

//CONEXION BROKER
void* escuchaPermanenteBroker(void* idConexionPermanente);
void* reconexionBroker();


#endif /* GAMECARD_H_ */