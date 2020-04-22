
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

t_list* inicializarEntrenadores() {

	char** objetivosEntrenadores = config_get_array_value(config,"OBJETIVOS_ENTRENADORES");
	char** pokemonEntrenadores = config_get_array_value(config,"POKEMON_ENTRENADORES");
	char** posicionesEntrenadores = config_get_array_value(config,"POSICIONES_ENTRENADORES");

	uint32_t size = 0;
	t_list* entrenadores = list_create();
	while(posicionesEntrenadores[size] != NULL) {
		size++;
	}
	uint32_t sizeTotal = size/2;
	uint32_t j=0; uint32_t k=0;	uint32_t l=0;

	printf("Size total: %i\n",sizeTotal);

	for(uint32_t i=0; i < sizeTotal; i++) {
		t_list* objetivos = list_create();
		t_list* pokemon = list_create();
		char* x = string_substring(posicionesEntrenadores[k], 1, 2); k++;
		char* y = string_substring(posicionesEntrenadores[k], 0, 1); k++;
		t_posicion* posicion = crearPosicion((uint32_t)atoi(x), (uint32_t)atoi(y));

		if((string_starts_with(objetivosEntrenadores[j],"[") == 1) && (string_ends_with(objetivosEntrenadores[j],"]") == 1)) {
			char* objetivoAlone = string_substring(objetivosEntrenadores[j], 1, strlen(objetivosEntrenadores[j]));
			list_add(objetivos, string_substring(objetivoAlone, 0, strlen(objetivoAlone)-1));
		} else {
			list_add(objetivos, string_substring(objetivosEntrenadores[j], 1, strlen(objetivosEntrenadores[j])));
			j++;
			while(string_ends_with(objetivosEntrenadores[j],"]") != 1) {
				list_add(objetivos, objetivosEntrenadores[j]);
				j++;
			}
			list_add(objetivos, string_substring(objetivosEntrenadores[j], 0, strlen(objetivosEntrenadores[j])-1));
		}
		j++;


		if((string_starts_with(pokemonEntrenadores[l],"[") == 1) && (string_ends_with(pokemonEntrenadores[l],"]") == 1)) {
			char* pokemonAlone = string_substring(pokemonEntrenadores[l], 1, strlen(pokemonEntrenadores[l]));
			list_add(objetivos, string_substring(pokemonAlone, 0, strlen(pokemonAlone)-1));
		} else {
			list_add(pokemon, string_substring(pokemonEntrenadores[l], 1, strlen(pokemonEntrenadores[l])));
			l++;
			while(string_ends_with(pokemonEntrenadores[l],"]") != 1) {
				list_add(pokemon, pokemonEntrenadores[l]);
				l++;
			}
			list_add(pokemon, string_substring(pokemonEntrenadores[l], 0, strlen(pokemonEntrenadores[l])-1));
		}
		l++;

		t_entrenador* entrenador = crearEntrenador(posicion, objetivos, pokemon);


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

t_entrenador* crearEntrenador(t_posicion* posicion, t_list* objetivos, t_list* pokemon) {
    t_entrenador* entrenador = malloc(sizeof(t_entrenador));

    entrenador->id = getNuevoPid();
	entrenador->posicion = posicion;
    entrenador->pokemonCapturados = list_duplicate(pokemon);
    entrenador->pokemonObjetivo = list_duplicate(objetivos);
    entrenador->pokemonPlanificado = NULL;
    entrenador->deadlock = 0;

    return entrenador;
}