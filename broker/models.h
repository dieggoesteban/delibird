#ifndef MODELS_H_
#define MODELS_H_
#define ERROR -1

#include "global-includes.h"

#pragma region MessageQueue
typedef enum
{
	NEW_POKEMON = 1,
	APPEARED_POKEMON = 2,
	CATCH_POKEMON = 3,
	CAUGHT_POKEMON = 4,
	GET_POKEMON = 5,
	LOCALIZED_POKEMON = 6,
} mq_cod;

//Basicamente, las colas de mensaje, tambien representan mensajes
typedef enum 
{
    SUBSCRIBE = 7,
	MENSAJE_RECIBIDO = 8,
	ACKNOWLEDGEMENT = 9,
	CONFIRMACION_MSJ = 10
} operation_cod;

typedef struct {
	char name[50];
    uint32_t mq_cod;
    t_list* mensajes;
    t_list* subscribers;

	//Semaphores
	pthread_mutex_t s_mensajes;
	pthread_mutex_t s_subscribers;
	sem_t s_hayMensajes;

	//Threads
	pthread_t dispatchMessagesThread;
} t_message_queue;

typedef struct{
	uint32_t id;
	uint32_t idCorrelativo;
	uint32_t mq_cod; //Cada mensaje sabe que tipo es
	uint32_t countSuscriptoresObjetivo;
	
	t_list* suscriptoresEnviados;
	t_list* suscriptoresConfirmados; 
	void* mensaje; //Contenido del mensaje
	
	//Semaphores
	pthread_mutex_t s_suscriptoresEnviados;
	pthread_mutex_t s_suscriptoresConfirmados;
	sem_t s_puedeEliminarse;

	//Threads
	pthread_t caching;
	pthread_t deleteFromQueue;
	
} t_message;
#pragma endregion

#pragma region Comunicacion
typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	mq_cod codigo_mensaje;
	t_buffer* buffer;
} t_paquete;
#pragma endregion

#pragma region Estructuras_Auxiliares
typedef struct
{
	uint32_t posicion_x;
	uint32_t posicion_y;
	uint32_t cantidad;
} t_posicion_cantidad;

typedef struct
{
	uint32_t posicion_x;
	uint32_t posicion_y;
} t_posicion;

typedef struct
{
    char* nombre;
    t_posicion* posicion;
} t_pokemon_posicion;

typedef struct{
    char* nombre;
    uint32_t cantidad;
}t_pokemon_cantidad;
#pragma endregion

#pragma region Mensajes_Deserializados

typedef struct {
	uint32_t messageQueue;
	uint32_t moduleId;
} t_register_module;

typedef struct{
	uint32_t id_mensajeEnviado;
} t_id_mensaje_recibido;

typedef struct{
	uint32_t idModule;
	uint32_t idMessageReceived;
	uint32_t mq;
} t_acknowledgement;

typedef struct{
	uint32_t idModule;
	uint32_t socket;
} t_suscripcion;

//Colas de mensajes

typedef struct
{
	uint32_t ID_mensaje_recibido;
	uint32_t sizeNombre;
	char* nombre;
	t_posicion_cantidad* posicionCantidad;
} t_new_pokemon;

typedef struct{
	uint32_t ID_mensaje_recibido;
	uint32_t ID_mensaje_original;
	uint32_t sizeNombre;
	char* nombre;
	uint32_t cantidadPosiciones; 
	t_list* posiciones; //una lista de t_posicion
}t_localized_pokemon;

typedef struct{
	uint32_t ID_mensaje_recibido;
	uint32_t sizeNombre;
	char* nombre;
	t_posicion* posicion;
} t_appeared_pokemon;

typedef struct{
	uint32_t ID_mensaje_recibido;
	uint32_t sizeNombre;
	char* nombre;
	t_posicion* posicion;
}t_catch_pokemon;

typedef struct{
	uint32_t ID_mensaje_recibido;
	uint32_t ID_mensaje_original;
	uint32_t catchStatus; 
}t_caught_pokemon;

typedef struct{
	uint32_t ID_mensaje_recibido;
	uint32_t sizeNombre;
	char* nombre;
} t_get_pokemon;
#pragma endregion

#pragma region Mensajes_Estructura_Cache

typedef struct {
	t_message* message;
	void* cacheStructure;
} cache_message;

typedef struct {
	uint32_t nameLength;
	char* pokeName;
	uint32_t posX;
	uint32_t posY;
	uint32_t cantidad;
} cache_new_pokemon;

typedef struct {
	uint32_t nameLength;
	char* pokeName;
	uint32_t cantidadPos;
	uint32_t* posiciones;
} cache_localized_pokemon;

typedef struct {
	uint32_t nameLength;
	char* pokeName;
} cache_get_pokemon;

typedef struct {
	uint32_t nameLength;
	char* pokeName;
	uint32_t posX;
	uint32_t posY;
} cache_appeared_pokemon;

typedef struct {
	uint32_t nameLength;
	char* pokeName;
	uint32_t posX;
	uint32_t posY;
} cache_catch_pokemon;

typedef struct {
	uint32_t seAtrapo;
} cache_caught_pokemon;
#pragma endregion

#endif /* MODELS_H_ */