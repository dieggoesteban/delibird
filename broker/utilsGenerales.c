#include "utilsGenerales.h"


//Solo lista de chars
bool perteneceALista(char *val, t_list* lista) {
	for (uint32_t i = 0; i < list_size(lista); i++) {
		char* word = list_get(lista, i);
		if (strcmp(word, val) == 0) {
			return true;
		}
	}
	return false;
}

//pasa los elementos de una lista a una queue
t_queue* listToQueue(t_list* lista){
	t_queue* queue = queue_create();
	for(uint32_t i = 0; i < list_size(lista); i++){
		queue_push(queue, list_get(lista, i));
	}
	return queue;
}

//te tira el index de ese elemento en la lista (siendo una lista de chars)
uint32_t perteneceAListaContador(char *val, t_list* lista) {
	uint32_t counter = 0;
	for (uint32_t i = 0; i < list_size(lista); i++) {
		char* word = list_get(lista, i);
		if (strcmp(word, val) == 0) {
			counter++;
		}
	}
	return counter;
}

//size del array
uint32_t arraySize(void* arr[]) {
	uint32_t size = 0;
	while(arr[size] != NULL) {
		size++;
	}
	return size;
}

//we array a lista
t_list* arrayToList(void* arr[]) {
	t_list* list = list_create();
	for(uint32_t i = 0; i < arraySize(arr); i++) {
		list_add(list, arr[i]);
	}
	return list;
}

//si listas son iguales
bool list_equals(t_list* list1, t_list* list2) {
	if(list_size(list1) == list_size(list2)) {
		for(uint32_t i = 0; i < list_size(list1); i++) {
			if(strcmp(list_get(list1,i),list_get(list1,2)) != 0) {
				return false;
			}
		}
		return true;
	} else return false;
}
