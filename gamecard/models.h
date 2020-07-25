
#ifndef MODELS_H_
#define MODELS_H_

#define ERROR -1
#include "semaphore.h"


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
	ID_ASIGNADO_SUSCRIPCION = 10
} operation_cod;



typedef struct{
	sem_t semPoke;
	char* nombrePoke;
} t_semaforo_pokemon;

typedef struct {
    uint32_t mq_cod;
    t_list* mensajes;
    t_list* suscribers;
} t_message_queue;

//struct que se manda como confirmacion de recibo de mensaje por parte de los modulos hacia el broker
typedef struct{
	uint32_t ID_mensaje;
	uint32_t MessageQueue;
	bool meLlego;
} t_confirmacion_mensaje;

typedef struct{
	t_list* suscriptoresConfirmados; //los suscriptores a los que se les envio el mensaje y tiraron una confirmacion 
	void* mensaje; //el struct que toque
} t_message;

typedef struct{
	uint32_t idModule;
	uint32_t idMessageReceived;
	uint32_t mq;
} t_acknowledgement;

typedef struct 
{
	uint32_t messageQueue;
	uint32_t moduleId;
} t_register_module;

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
	uint32_t ID_mensaje_recibido;
	uint32_t sizeNombre;
	char* nombre;
	t_posicion_cantidad* posicionCantidad;
} t_new_pokemon;

typedef struct{
	uint32_t indexSemaforo;
	t_new_pokemon* newPokemon;
}t_newPokemon_indexSem;

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
	uint32_t ID_mensaje_original;
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
	uint32_t indexSemaforo;
	t_catch_pokemon* catchPokemon;
}t_catchPokemon_indexSem;

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

typedef struct{
	uint32_t indexSemaforo;
	t_get_pokemon* getPokemon;
}t_getPokemon_indexSem;

typedef struct
{
    char* nombre;
    t_posicion* posicion;
} t_pokemon_posicion;

typedef struct{
    char* nombre;
    uint32_t cantidad;
}t_pokemon_cantidad;

typedef struct
{
	uint32_t id;
	t_posicion* posicion;
    t_list* pokemonCapturados;
    t_list* pokemonObjetivo;
    uint32_t cantidadObjetivo;
    t_pokemon_posicion* pokemonPlanificado;
	bool enEspera;
    bool deadlock; 
} t_entrenador;

typedef struct
{
	uint32_t conexion;
	uint32_t messageQueue;
} t_suscribe;

typedef struct {
	uint32_t id_mensajeEnviado;
} t_id_mensaje_recibido;

#endif /* MODELS_H_ */