#ifndef SERIALIZADOR_H_
#define SERIALIZADOR_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include "utils.h"
#include "models.h"
//METODOS DE CONEXION
// int crear_conexion(char* ip, char* puerto);
// void enviarMensaje(t_paquete* paquete, uint32_t socket_cliente);
// char* recibir_mensaje(int socket_cliente);
// void liberar_conexion(int socket_cliente);

//PAQUETES
t_paquete* getPaquete(char* arrayArgumentos[], char* tipo_mensaje);
t_paquete* modoSuscriptor(uint32_t mq_cod);
void liberarPaquete(t_paquete* paquete);
void* serializar_paquete(t_paquete* paquete, int bytes);

//SERIALIZACION Y DESERIALIZACION
t_new_pokemon* deserializar_newPokemon(t_buffer* buffer);
t_paquete* serializar_newPokemon(t_new_pokemon* newPokemon);
t_paquete* serializar_appearedPokemon(t_appeared_pokemon* pokemon);
t_appeared_pokemon* deserializar_appearedPokemon(t_buffer* buffer);
t_paquete* serializar_catchPokemon(t_catch_pokemon* pokemon);
t_catch_pokemon* deserializar_catchPokemon(t_buffer* buffer);
t_paquete* serializar_caughtPokemon(t_caught_pokemon* pokemon);
t_caught_pokemon* deserializar_caughtPokemon(t_buffer* buffer);
t_paquete* serializar_getPokemon(t_get_pokemon* pokemon);
t_get_pokemon* deserializar_getPokemon(t_buffer* buffer);
t_paquete* serializar_localizedPokemon(t_localized_pokemon* localizedPokemon);
t_localized_pokemon* deserializar_localizedPokemon(t_buffer* buffer);

t_paquete* serializar_registerModule(t_register_module* registerModule);
t_register_module* deserializar_registerModule(t_buffer* buffer);

t_paquete* serializar_idSubscriberAssigned(t_id_subscriber_assigned* id);
t_id_subscriber_assigned* deserializar_idSubscriberAssigned(t_buffer* buffer);

t_paquete* serializar_acknowledgement(t_acknowledgement* akc);
t_acknowledgement* deserializar_acknowledgement(t_buffer* buffer);

t_paquete* serializar_idMensajeRecibido(t_id_mensaje_recibido* idMensajeRecibido);
t_id_mensaje_recibido* deserializar_idMensajeRecibido(t_buffer* buffer);

//CREACION DE LOS STRUCTS
t_posicion_cantidad* crearPosicionCantidad(uint32_t x, uint32_t y, uint32_t cant);
t_posicion* crearPosicion(uint32_t x, uint32_t y);
t_new_pokemon* crearNewPokemon(uint32_t IDMensajeRecibido, char* nombre, t_posicion_cantidad* posicionCantidad);
t_localized_pokemon* crearLocalizedPokemon(uint32_t IDMensajeRecibido,uint32_t IDMensajeOriginal, char* nombre, uint32_t sizePosicionCantidad, t_list* posicion);
t_appeared_pokemon* crearAppearedPokemon(uint32_t IDMensajeRecibido, uint32_t IDMensajeOriginal, char* nombre, t_posicion* posicion);
t_catch_pokemon* crearCatchPokemon(uint32_t ID_mensaje_recibido, char* nombre, t_posicion* posicion);
t_caught_pokemon* crearCaughtPokemon(uint32_t IDMensajeRecibido, uint32_t IDMensajeOriginal, uint32_t catchStatus);
t_get_pokemon* crearGetPokemon(uint32_t ID_mensaje_recibido, char* nombre);
t_register_module* crearSuscribe(uint32_t ID_message_queue);
t_acknowledgement* crearAcknowledgement(uint32_t ID_mensaje_recibido, uint32_t mq_cod);
t_id_mensaje_recibido* crearIdMensajeRecibido(uint32_t id);
t_id_subscriber_assigned* crearIdSubscriberAssigned(uint32_t socket_cliente);

#endif /* SERIALIZADOR_H_ */