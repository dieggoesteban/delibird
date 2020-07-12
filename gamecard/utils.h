
#ifndef UTILS_H_
#define UTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<commons/string.h>
#include<commons/config.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include "models.h"
#include "commons/collections/dictionary.h"
#include "serializador.h"

t_log* logger;
t_config* config;


uint32_t procesarComando(char** ip, char** puerto, char *proceso, char *tipo_mensaje);
void cortarArgumentos(int lengthArray, char *argumentos[], char *arrayTemp[]);
uint32_t perteneceAlArray(char* val, char* arr[], uint32_t size);
t_dictionary* crearDictionaryDeTexto(char* textoArchivo);
void add_cofiguration(char *line);
char *obtenerStringDic(t_dictionary* dic, char *key);
int obtenerIntDic(t_dictionary *dic, char *key);
long obtenerLongDic(t_dictionary *dic, char *key);
double obtenerDoubleDic(t_dictionary *dic, char *key);
char** obtenerArrayCharDic(t_dictionary* dic, char* key);
void removerKeyDic(t_dictionary *dic, char *key);
void setValueDic(t_dictionary *dic, char *key, char *value);
char* dictionaryToString(t_dictionary* dictionary);
t_list* dictionaryToListPosiciones(t_dictionary* dictionary);

#endif /* UTILS_H_ */
