
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

u_int32_t getCantidadEntrenadores() {
	char** posicionesEntrenadores = config_get_array_value(config,"POSICIONES_ENTRENADORES");
	uint32_t size = 0;
	while(posicionesEntrenadores[size] != NULL) {
		size++;
	}
	free(posicionesEntrenadores);
	return size/2;
}

t_list* inicializarEntrenadores() {

	char** objetivosEntrenadores = config_get_array_value(config,"OBJETIVOS_ENTRENADORES"); //Obtiene el array desde config, pero mal. Labura como un split de un string, separa por comas y eso es un elemento del array que genera. Por eso mismo el algoritmo feo de abajo
	char** pokemonEntrenadores = config_get_array_value(config,"POKEMON_ENTRENADORES");
	char** posicionesEntrenadores = config_get_array_value(config,"POSICIONES_ENTRENADORES");

	t_list* entrenadores = list_create();

	uint32_t sizeTotal = getCantidadEntrenadores(); //sabemos que las posiciones siempre van en pares, por eso contamos cuantos elementos hay en el array traido de config y lo dividimos por dos
	uint32_t j=0; uint32_t k=0;	uint32_t l=0; //j es para objetivos de los entrenadores
											 // k es para las posiciones de los entrenadores
											//  l es para los pokemones de los entrenadores
	uint32_t sizeAux = 0;

	printf("Size total: %i\n",sizeTotal);

	for(uint32_t i=0; i < sizeTotal; i++) {
		t_list* objetivos = list_create();
		t_list* pokemon = list_create();
		char* x = string_substring(posicionesEntrenadores[k], 1, 2); k++; //empieza el substring en 1 ya que queremos sacar el [ que queda pegado al elemento en el array traido del config
		char* y = string_substring(posicionesEntrenadores[k], 0, 1); k++; //empieza el substring en 0 y hace length de un solo caracter para sacar el ] que queda pegado al elemento en el array traido del config
		t_posicion* posicion = crearPosicion((uint32_t)atoi(x), (uint32_t)atoi(y));

		if((string_starts_with(objetivosEntrenadores[j],"[") == 1) && (string_ends_with(objetivosEntrenadores[j],"]") == 1)) { //este if considera el caso en el entrenador solo tiene un pokemon objetivo, es decir que viene asi del array: [Bulbasaur]
			char* objetivoAlone = string_substring(objetivosEntrenadores[j], 1, strlen(objetivosEntrenadores[j]));
			list_add(objetivos, string_substring(objetivoAlone, 0, strlen(objetivoAlone)-1));
		} else {
			list_add(objetivos, string_substring(objetivosEntrenadores[j], 1, strlen(objetivosEntrenadores[j]))); //este contempla el primer elemento del array, que viene con '['
			j++;
			while(string_ends_with(objetivosEntrenadores[j],"]") != 1) { //se hace el while mientras no se llegue al elemento final
				list_add(objetivos, objetivosEntrenadores[j]); //este contempla el caso de los elementos que estan en el medio, que no contienen ni '[' ó ']'
				j++;
			}
			list_add(objetivos, string_substring(objetivosEntrenadores[j], 0, strlen(objetivosEntrenadores[j])-1)); //agrega el ultimo elemento del array
		}
		j++;

		uint32_t sizeObjetivos = j - sizeAux; //sirve para poder determinar la cantidad maxima que puede tener un entrenador
		sizeAux = j;


		if((string_starts_with(pokemonEntrenadores[l],"[") == 1) && (string_ends_with(pokemonEntrenadores[l],"]") == 1)) {//este if considera el caso en el entrenador solo tiene un pokemon objetivo, es decir que viene asi del array: [Bulbasaur]
			char* pokemonAlone = string_substring(pokemonEntrenadores[l], 1, strlen(pokemonEntrenadores[l]));
			list_add(pokemon, string_substring(pokemonAlone, 0, strlen(pokemonAlone)-1));
		} else {
			list_add(pokemon, string_substring(pokemonEntrenadores[l], 1, strlen(pokemonEntrenadores[l]))); //este contempla el primer elemento del array, que viene con '['
			l++;
			while(string_ends_with(pokemonEntrenadores[l],"]") != 1) { //se hace el while mientras no se llegue al elemento final
				list_add(pokemon, pokemonEntrenadores[l]); //este contempla el caso de los elementos que estan en el medio, que no contienen ni '[' ó ']'
				l++;
			}
			list_add(pokemon, string_substring(pokemonEntrenadores[l], 0, strlen(pokemonEntrenadores[l])-1)); //agrega el ultimo elemento del array
		}
		l++;

		t_entrenador* entrenador = crearEntrenador(posicion, objetivos, pokemon, sizeObjetivos);


		list_add(entrenadores, entrenador);

		printf("valor final en la iteracion %i: \n", i);
		printf("Objetivos j: %i \n",j);
		printf("Posiciones k: %i \n",k);
		printf("Entrenadores l: %i \n",l);

		list_destroy(objetivos);
		list_destroy(pokemon);
	}

	return entrenadores;	
}


void setObjetivoGlobal(t_list* entrenadores){
	t_list* pokemonesObjetivos = list_create();

	t_list* objetivoGlobal = list_create();
	t_list* globalAux = list_create();

	for(uint32_t i = 0; i < list_size(entrenadores); i++){
		list_add_all(pokemonesObjetivos, ((t_entrenador*)list_get(entrenadores, i))->pokemonObjetivo);
	}

	for(uint32_t i = 0; i < list_size(entrenadores); i++){
		t_list* pokemon = list_duplicate(((t_entrenador*)list_get(entrenadores, i))->pokemonObjetivo);

		for(uint32_t j = 0; j < list_size(pokemon); j++){
			printf("%s\n",(char*)list_get(pokemon, j));
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

	printf("Objetivo Global \n");

	for (uint32_t i = 0; i < list_size(objetivoGlobal); i++) {
		t_pokemon_cantidad* poke = (t_pokemon_cantidad*)list_get(objetivoGlobal, i);
		
		printf("Objetivo %i: %s cantidad %i \n",i+1,(char*)poke->nombre,(uint32_t)poke->cantidad);
	}
	
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