#ifndef TALLGRASS_H_
#define TALLGRASS_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include<fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>
#include <pthread.h>

#include<commons/string.h>
#include<commons/collections/list.h>
#include<commons/config.h>
#include<commons/bitarray.h>
#include "commons/collections/dictionary.h"

#include "utils.h"
#include "models.h"
#include "utilsGenerales.h"
#include "sockets.h"


//GLOBALES
t_config* config;
t_log* logger;
char* punto_montaje;
char* pathFiles;
char* pathBlocks;
char* pathBitmap;
char* pathMetadata;
void* bmap;
t_bitarray* bitmapArr;
uint32_t cantBloques;
uint32_t sizeBloque;
char* magicNumber;
t_list* semaforosPokemon;
uint32_t tiempoReintentoConexion;
uint32_t tiempoReintentoOperacion;
uint32_t tiempoOperacion;

pthread_mutex_t mutexBitmap;
pthread_mutex_t yaExistiaDirec;
pthread_mutex_t mutexEstaOpen;

//CREAR
bool crearDirectorio(char* rutaDirectorio);
t_bitarray* crear_bitmap_en_disco(char* archivo, size_t size);
t_bitarray *copiar_bitmap_en_disco(char* archivo, size_t size);

//AGREGAR EN PATH
char* agregarAPuntoMontaje(char* restoRuta);
char* agregarADirectorioBlocksInt(uint32_t nroBloque);
char* agregarADirectorioBlocksChar(char* nroBloque);
char* agregarADirectorioFiles(char* restoRuta);
char* agregarADirectorioBitmap(char* restoRuta);
char* agregarADirectorioMetadataFS(char* restoRuta);
char* agregarAPath(char* path, char* agregado);

//ATENDER
void atenderGetPokemon(t_getPokemon_indexSem* getPokemon);
void* atenderNewPokemon(void* newPokemon);
void atenderCatchPokemon(void* catchPokemon);

//ESCRIBIR
void escribirBloque(char* texto, char* pathMetadataPoke, uint32_t bloqueAEscribir, char* modoEscritura);
void escribirMetadata(char* path, char* aEscribir);
void escribirNewPokemon(char *textoNewPoke, char* pathMetadataPoke);
uint32_t escribirCatchPokemon(char *textoCatchPokemon, char* pathMetadataPoke);
t_localized_pokemon* escribirLocalizedPokemon(t_get_pokemon* getPoke, char* pathMetadataPoke);

//LEER
char* leerMetadata(char* path);
void leerCantPosicion(char* nombrePoke, char* posicion);
char* leerArchivo(char *pathMetadataPoke);

//A TEXTO
char* newPokemonATexto(t_new_pokemon* newPokemon);
char* catchPokemonATexto(t_catch_pokemon* catchPokemon);

//INSERTAR
void insertarMetadataEnPath(char* pathDirectorio, char* archivoMetadata);
void insertarBloqueEnArchivo(t_config* metadataPoke, char** arrBloques, uint32_t bloque);
void setSizeArchivo(t_config* metadataPoke, uint32_t sizeNuevo);
void copiarArchivo(char* sourcePath, char* destinationPath);

//MODIFICAR
void modificarBloquesNewPoke(char* textoCompleto, char* pathMetadataPoke);
void modificarBloquesCatchPoke(char* textoCompleto, char* pathMetadataPoke, uint32_t bytesBorrados);
// void modificarOpenArchivo(t_config* metadataPoke, char* valorOpen);
void modificarOpenArchivo(char* metadataPoke, char* valorOpen);

//CALCULOS Y BUSQUEDAS
uint32_t buscarBloqueLibre();
uint32_t verQueBloqueEscribir(char* pathMetadataPoke,int* sizeAEscribirPrimerBloque, bool modificarBloques, uint32_t j);
void separarTextoEnBloques(char* textoArchivo, char* pathMetadataPoke, bool modificarBloques, char* modoEscritura);
int calculoBytesSobrantes(char** arrBloques, uint32_t sizeArchivo);
uint32_t buscarBloqueEInsertarEnArchivo(t_config* metadataPoke);
void sacarUltimoBloqueDeArchivo(t_config* metadataPoke, char** arrBloques);
bool existeBitmap(char* rutaArchivo);
bool existeDirectorio(char* rutaDirectorio);
bool existeArchivo(char* rutaDirectorio);
bool estaOpen(char* metadataPoke);
void* reintentarOperacion(void* metadataPoke);
void reintentandoOperacion(char* metadataPoke);
void waitSemYModificacionOpen(uint32_t indexSemaforo, char* pathMetadataPoke);
void signalSemYModificacionOpen(uint32_t indexSemaforo, char* pathMetadataPoke);


#endif /* TALLGRASS_H_ */