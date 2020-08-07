#ifndef MODELS_H_
#define MODELS_H_

#include<semaphore.h>

#define ERROR -1

typedef enum
{
	NEW_POKEMON = 1,
	APPEARED_POKEMON = 2,
	CATCH_POKEMON = 3,
	CAUGHT_POKEMON = 4,
	GET_POKEMON = 5,
	LOCALIZED_POKEMON = 6,
    SUSCRIBE = 7
} mq_cod;

typedef enum 
{
    SUBSCRIBE = 7,
	MENSAJE_RECIBIDO = 8,
	ACKNOWLEDGEMENT = 9,
	CONFIRMACION_MSJ = 10,
	DESCONEXION = 11
} operation_cod;

//struct que se manda como confirmacion de recibo de mensaje por parte de los modulos hacia el broker
typedef struct{
	uint32_t ID_mensaje;
	uint32_t MessageQueue;
	bool meLlego;
} t_confirmacion_mensaje;

typedef struct {
	uint32_t id_mensajeEnviado;
} t_id_mensaje_recibido;

typedef struct {
	uint32_t AKC;
} t_akc;

typedef struct {
	uint32_t idEntrenador;
	bool deadlock;
} t_trDeadlock;

typedef struct {
	t_list* entrenadores;
	uint32_t cantIntercambios;
	bool estaResuelto;
} t_deadlock;

typedef struct {
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
	uint32_t conexion;
	uint32_t messageQueue;
} t_suscribe;

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
} t_catch_pokemon;

typedef struct{
	uint32_t idModule;
	uint32_t idMessageReceived;
	uint32_t mq;
} t_acknowledgement;

typedef struct{
	uint32_t ID_mensaje_recibido;
	uint32_t ID_mensaje_original;
	uint32_t catchStatus; 
} t_caught_pokemon;

typedef struct{
	uint32_t ID_mensaje_recibido;
	uint32_t sizeNombre;
	char* nombre;
} t_get_pokemon;

typedef struct{
    char* nombre;
    uint32_t cantidad;
}t_pokemon_cantidad;

typedef struct
{
    char* nombre;
    t_posicion* posicion;
	uint32_t tiempoEjecucion;
	uint32_t tiempoCaptura;
} t_pokemon_posicion;

typedef struct {
	uint32_t id;
	t_posicion* posicion;
	uint32_t tiempoEjecucion;
	uint32_t tiempoIntercambio;
}t_entrenador_posicion;

typedef struct
{
	uint32_t id;
	t_posicion* posicion;
    t_list* pokemonCapturados;
    t_list* pokemonObjetivo;
    uint32_t cantidadObjetivo;
	uint32_t estimacionAnterior;
    t_pokemon_posicion* pokemonPlanificado;
	t_entrenador_posicion* entrenadorPlanificado;
	bool enEspera;
    bool deadlock;
	sem_t mutex;
	uint32_t cantCiclosCPU;
} t_entrenador;

typedef struct
{
	uint32_t entrenadorID;
	uint32_t mensajeId;
} t_entrenador_catch;

#endif /* MODELS_H_ */