
#include "utils.h"


uint32_t perteneceAlArray(char *val, char *arr[], uint32_t size)
{
	for (uint32_t i = 0; i < size; i++)
	{
		if (strcmp(arr[i], val) == 0)
			return 1;
	}
	return 0;
}

uint32_t perteneceALista(char *val, t_list* lista) {
	for (uint32_t i = 0; i < list_size(lista); i++) {
		char* word = list_get(lista, i);
		if (strcmp(word, val) == 0) {
			return 1;
		}
	}
	return 0;
}

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

uint32_t arraySize(void* arr[]) {
	uint32_t size = 0;
	while(arr[size] != NULL) {
		size++;
	}
	return size;
}

t_list* arrayToList(void* arr[]) {
	t_list* list = list_create();
	for(uint32_t i = 0; i < arraySize(arr); i++) {
		list_add(list, arr[i]);
	}
	return list;
}

void inicializarEntrenadores() {
	char** objetivosEntrenadores = config_get_array_value(config,"OBJETIVOS_ENTRENADORES"); //Obtiene el array desde config, pero mal. Labura como un split de un string, separa por comas y eso es un elemento del array que genera. Por eso mismo el algoritmo feo de abajo
	char** pokemonEntrenadores = config_get_array_value(config,"POKEMON_ENTRENADORES");
	char** posicionesEntrenadores = config_get_array_value(config,"POSICIONES_ENTRENADORES");

	for(uint32_t i = 0; i < arraySize((void*)objetivosEntrenadores); i++) {

		t_list* objetivos = arrayToList((void*)string_split(objetivosEntrenadores[i],"|"));
		t_list* pokemon = arrayToList((void*)string_split(pokemonEntrenadores[i],"|"));
		
		char** coordenadas = string_split(posicionesEntrenadores[i],"|");
		t_posicion* posicion = crearPosicion((uint32_t)atoi(coordenadas[0]),(uint32_t)atoi(coordenadas[1]));

		uint32_t sizeObjetivos = arraySize((void*)string_split(objetivosEntrenadores[i],"|"));

		list_add(colaNEW, crearEntrenador(posicion, objetivos, pokemon, sizeObjetivos));

		list_destroy(objetivos);
		list_destroy(pokemon);
		free(coordenadas);
	}

}

void setObjetivoGlobal(){
	t_list* pokemonesObjetivos = list_create();
	t_list* globalAux = list_create();

	objetivoGlobal = list_create();

	for(uint32_t i = 0; i < list_size(colaNEW); i++){
		list_add_all(pokemonesObjetivos, ((t_entrenador*)list_get(colaNEW, i))->pokemonObjetivo);
	}

	for(uint32_t i = 0; i < list_size(colaNEW); i++){
		t_list* pokemon = list_duplicate(((t_entrenador*)list_get(colaNEW, i))->pokemonObjetivo);

		for(uint32_t j = 0; j < list_size(pokemon); j++){
			if(perteneceALista(list_get(pokemon, j),globalAux) == 0) {
				uint32_t cant = perteneceAListaContador(list_get(pokemon, j),pokemonesObjetivos);
				list_add(objetivoGlobal, setPokemonCantidad(list_get(pokemon, j), cant));
				list_add(globalAux, list_get(pokemon, j));
			}
		}
		free(pokemon);
	}

	free(pokemonesObjetivos);
	free(globalAux);
}

t_entrenador* crearEntrenador(t_posicion* posicion, t_list* objetivos, t_list* pokemon, uint32_t cantObjetivos) {
    t_entrenador* entrenador = malloc(sizeof(t_entrenador));

    entrenador->id = getNuevoPid();
	entrenador->posicion = posicion;
    entrenador->pokemonCapturados = list_duplicate(pokemon);
    entrenador->pokemonObjetivo = list_duplicate(objetivos);
    entrenador->pokemonPlanificado = NULL;
	entrenador->cantidadObjetivo = cantObjetivos;
    entrenador->deadlock = 0;

    return entrenador;
}

t_pokemon_cantidad* setPokemonCantidad(char* nombre, uint32_t cantidad) {
	t_pokemon_cantidad* pokemon = malloc(sizeof(t_pokemon_cantidad));

	pokemon->nombre = nombre;
	pokemon->cantidad = cantidad;

	return pokemon;
}