#ifndef UTILSGENERALES_H_
#define UTILSGENERALES_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<commons/string.h>
#include<commons/config.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<commons/collections/queue.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>

#include "models.h"
#include "serializador.h"

//UTILIDADES ARRAYS/LISTS/QUEUE

uint32_t arraySize(void* arr[]);
t_list* arrayToList(void* arr[]);
char* arrayToString(void* arr[]);
bool perteneceALista(char *val, t_list* lista);
uint32_t perteneceAListaContador(char *val, t_list* lista);
bool list_equals(t_list* list1, t_list* list2);
t_queue* listToQueue(t_list* lista);
char* listToString(t_list* lista);

#endif /* UTILSGENERALES_H_ */