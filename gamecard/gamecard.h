#ifndef GAMECARD_H_
#define GAMECARD_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include<commons/bitarray.h>

#include "utils.h"
#include "sockets.h"
#include "models.h"
#include "semaphore.h"
#include "tallgrass.h"


//GLOBALES
t_config* config;
t_log* logger;
char* IP;
char* PUERTO;
uint32_t tiempoReintentoConexion;
char* punto_montaje;
char* pathFiles;
char* pathBlocks;
char* pathBitmap;
char* pathMetadata;

t_bitarray* bitmapArr;
uint32_t cantBloques;
uint32_t sizeBloque;
char* magicNumber;

//SEMAFOROS
sem_t semReconexion;

//PROGRAMA
void inicializarGamecard();
void terminar_programa(int, t_log*, t_config*);

//CONEXION BROKER
void* escuchaPermanenteBroker(void* idConexionPermanente);
void* reconexionBroker();


#endif /* GAMECARD_H_ */