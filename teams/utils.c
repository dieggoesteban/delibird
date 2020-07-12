
#include "utils.h"


void inicializarPid() {
    pid = 0;
}

uint32_t getNuevoPid() {
    uint32_t nuevoPid = pid++;

    return nuevoPid;
}


bool perteneceAlArray(char *val, char *arr[], uint32_t size)
{
	for (uint32_t i = 0; i < size; i++)
	{
		if (strcmp(arr[i], val) == 0)
			return true;
	}
	return false;
}

bool perteneceALista(char *val, t_list* lista) {
	for (uint32_t i = 0; i < list_size(lista); i++) {
		char* word = list_get(lista, i);
		if (strcmp(word, val) == 0) {
			return true;
		}
	}
	return false;
}

t_queue* listToQueue(t_list* lista){
	t_queue* queue = queue_create();
	for(uint32_t i = 0; i < list_size(lista); i++){
		queue_push(queue, list_get(lista, i));
	}
	return queue;
}


uint32_t pokemonCantidadPerteneceALista(t_pokemon_cantidad* pokemon, t_list* lista){
	for(uint32_t i = 0; i < list_size(lista); i++){
		if(strcmp(((t_pokemon_cantidad*)list_get(lista, i))->nombre,pokemon->nombre) == 0){
			return i;
		}
	}
	return ERROR;
}

uint32_t pokemonPosicionPerteneceALista(t_pokemon_posicion* pokemon, t_list* lista){
	for(uint32_t i = 0; i < list_size(lista); i++){
		if(strcmp(((t_pokemon_posicion*)list_get(lista, i))->nombre,pokemon->nombre) == 0){
			return i;
		}
	}
	return ERROR;
}

uint32_t entrenadorPerteneceALista(t_entrenador* entrenador, t_list* lista){
	for(uint32_t i = 0; i < list_size(lista); i++){
		if(entrenador->id == (((t_entrenador*)list_get(lista,i))->id))
			return i;
	}
	return ERROR;
}

t_list* obtenerEntrenadoresSinDeadlock(){
	t_list* entrenadoresSinDeadlock = list_create();
	for(uint32_t i = 0; i < list_size(colaBLOCKED); i++){
		if(((t_entrenador*)list_get(colaBLOCKED, i))->deadlock == false){
			list_add(entrenadoresSinDeadlock, (t_entrenador*)list_get(colaBLOCKED, i));
			return entrenadoresSinDeadlock;
		}
	}
	return entrenadoresSinDeadlock;
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

bool ordenarEntrenador(void* a, void* b) {
	t_entrenador* tr1 = (t_entrenador*) a;
	t_entrenador* tr2 = (t_entrenador*) b;

	if((tr1->posicion->posicion_x == tr2->posicion->posicion_x)) {
		return (tr1->posicion->posicion_y < tr2->posicion->posicion_y);
	} else 
		return (tr1->posicion->posicion_x < tr2->posicion->posicion_x);
}

bool ordenarPokemon(void* a, void* b) {
	t_pokemon_posicion* pk1 = (t_pokemon_posicion*) a;
	t_pokemon_posicion* pk2 = (t_pokemon_posicion*) b;

	if((pk1->posicion->posicion_x == pk2->posicion->posicion_x)) {
		return (pk1->posicion->posicion_y < pk2->posicion->posicion_y);
	} else 
		return (pk1->posicion->posicion_x < pk2->posicion->posicion_x);
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
	entrenador->enEspera = false;
	entrenador->deadlock = entrenadorEnDeadlock(entrenador);

    return entrenador;
}

t_pokemon_posicion* crearPokemonPosicion(char* nombre, t_posicion* posicion){
	t_pokemon_posicion* pokemon = malloc(sizeof(t_pokemon_posicion));

	pokemon->nombre = nombre;
	pokemon->posicion = posicion;

	return pokemon;
}

t_pokemon_cantidad* setPokemonCantidad(char* nombre, uint32_t cantidad) {
	t_pokemon_cantidad* pokemon = malloc(sizeof(t_pokemon_cantidad));

	pokemon->nombre = nombre;
	pokemon->cantidad = cantidad;

	return pokemon;
}

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

bool entrenadorEnDeadlock(t_entrenador* entrenador){
	if(!entrenadorCumplioObjetivo(entrenador) && entrenador->cantidadObjetivo == list_size(entrenador->pokemonCapturados)) {
		return true;
	}
	return false;
}

bool entrenadorCumplioObjetivo(t_entrenador* entrenador) {
	if(list_equals(entrenador->pokemonObjetivo,entrenador->pokemonCapturados)) {
		return true;
	}
	return false;
}

bool entrenadorPuedeCapturar(void* entrenador) {
	t_entrenador* trainer = (t_entrenador*) entrenador;

	return (!entrenadorEnDeadlock(trainer) && !trainer->enEspera);
}

t_entrenador* cambiarPosicionEntrenador(t_entrenador* entrenador, uint32_t posX, uint32_t posY){
	entrenador->posicion->posicion_x = posX;
	entrenador->posicion->posicion_y = posY;
	return entrenador;
}

uint32_t turnosHastaPokemon(t_pokemon_posicion* pokemon, t_entrenador* entrenador){
	return abs(pokemon->posicion->posicion_x - entrenador->posicion->posicion_x) + abs(pokemon->posicion->posicion_y - entrenador->posicion->posicion_y);
}

