
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

	char** objetivosEntrenadores = config_get_array_value(config,"OBJETIVOS_ENTRENADORES"); //Obtiene el array desde config, pero mal. Labura como un split de un string, separa por comas y eso es un elemento del array que genera. Por eso mismo el algoritmo feo de abajo
	char** pokemonEntrenadores = config_get_array_value(config,"POKEMON_ENTRENADORES");
	char** posicionesEntrenadores = config_get_array_value(config,"POSICIONES_ENTRENADORES");

	uint32_t size = 0;
	t_list* entrenadores = list_create();
	while(posicionesEntrenadores[size] != NULL) {
		size++;
	}
	uint32_t sizeTotal = size/2; //sabemos que las posiciones siempre van en pares, por eso contamos cuantos elementos hay en el array traido de config y lo dividimos por dos
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
	t_list* nombresDiferentes = list_create();



	for(uint32_t i = 0; i < list_size(entrenadores);i++){
		list_add_all(pokemonesObjetivos, ((t_entrenador*)list_get(entrenadores, i))->pokemonObjetivo);
	}

	uint32_t indexNombresDif = 0;
	list_add(nombresDiferentes, list_get(pokemonesObjetivos, 0));
	// for(uint32_t j = 1; j < list_size(pokemonesObjetivos); j++){ //empieza en j en 1 porque seria al pedo analizar el caso de la primera posicion, ya que esta en la lista de nombres already
	// 	if(!list_any_satisfy(nombresDiferentes, (void*)(strcmp(list_get(nombresDiferentes,indexNombresDif), list_get(pokemonesObjetivos,j)) == 0))){
	// 		list_add(nombresDiferentes, list_get(pokemonesObjetivos,j));
	// 		indexNombresDif++;
	// 	}
	// }
	
	for(int l = 0; l < list_size(nombresDiferentes); l++){
		printf( "pokes distintos: %s",(char*)list_get(nombresDiferentes, l));
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