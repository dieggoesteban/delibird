
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

uint32_t getIndexList(char *val, t_list* lista) {
	for (uint32_t i = 0; i < list_size(lista); i++) {
		char* word = list_get(lista, i);
		if (strcmp(word, val) == 0) {
			return i;
		}
	}
	return ERROR;
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

uint32_t pokemonPosicionPerteneceALista(char* pokemon, t_list* lista){
	for(uint32_t i = 0; i < list_size(lista); i++){
		if(strcmp(((t_pokemon_posicion*)list_get(lista, i))->nombre,pokemon) == 0){
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
	t_list* pokemonEntrenadores = arrayToList((void*)config_get_array_value(config,"POKEMON_ENTRENADORES"));
	char** posicionesEntrenadores = config_get_array_value(config,"POSICIONES_ENTRENADORES");
	cantEntrenadores = arraySize((void*)objetivosEntrenadores);
	for(uint32_t i = 0; i < arraySize((void*)objetivosEntrenadores); i++) {

		t_list* objetivos = arrayToList((void*)string_split(objetivosEntrenadores[i],"|"));
		t_list* pokemon = list_create();
		if(list_get(pokemonEntrenadores, i) != NULL) {
			pokemon = arrayToList((void*)string_split(list_get(pokemonEntrenadores, i),"|"));
		}
		char** coordenadas = string_split(posicionesEntrenadores[i],"|");
		t_posicion* posicion = crearPosicion((uint32_t)atoi(coordenadas[0]),(uint32_t)atoi(coordenadas[1]));

		uint32_t sizeObjetivos = arraySize((void*)string_split(objetivosEntrenadores[i],"|"));

		t_entrenador* entrenador = crearEntrenador(posicion, objetivos, pokemon, sizeObjetivos);

		sem_init(&entrenador->mutex, 0, 0);

		list_add(colaNEW, entrenador);
		sem_post(&counterEntrenadoresDisponibles);
		list_destroy(objetivos);
		list_destroy(pokemon);
		free(coordenadas);
	}
}

uint32_t maxNum(uint32_t a, uint32_t b) {
	if(a-b >= 0) {
		return a-b;
	} else return 0;
}

t_list* getCaughtPokemon() {
	t_list* aux = list_create();
	for(uint32_t i = 0; i < list_size(colaNEW); i++){
		list_add_all(aux, ((t_entrenador*)list_get(colaNEW, i))->pokemonCapturados);
	}

	return aux;
}

void setObjetivoGlobal(){
	t_list* pokemonesObjetivos = list_create();
	t_list* globalAux = list_create();

	objetivoGlobal = list_create();

	t_list* caughtPokes = getCaughtPokemon();

	for(uint32_t i = 0; i < list_size(colaNEW); i++){
		list_add_all(pokemonesObjetivos, ((t_entrenador*)list_get(colaNEW, i))->pokemonObjetivo);
	}

	for(uint32_t i = 0; i < list_size(colaNEW); i++){
		t_list* pokemon = list_duplicate(((t_entrenador*)list_get(colaNEW, i))->pokemonObjetivo);

		for(uint32_t j = 0; j < list_size(pokemon); j++){
			if(perteneceALista(list_get(pokemon, j),globalAux) == 0) {
				uint32_t cant = perteneceAListaContador(list_get(pokemon, j),pokemonesObjetivos);
				uint32_t cantidad = perteneceAListaContador(list_get(pokemon,j),caughtPokes);
				list_add(objetivoGlobal, setPokemonCantidad(list_get(pokemon, j), maxNum(cant, cantidad)));
				list_add(globalAux, list_get(pokemon, j));
			}
		}
		free(pokemon);
	}

	free(pokemonesObjetivos);
	free(globalAux);
}

void actualizarObjetivoGlobal(char* poke, bool restar) {
	uint32_t index = pokemonPosicionPerteneceALista(poke,objetivoGlobal);
	t_pokemon_cantidad* pokemon = list_remove(objetivoGlobal,index);
	if(restar) {
		pokemon->cantidad = pokemon->cantidad - 1;
	} else {
		pokemon->cantidad = pokemon->cantidad + 1;
	}
	list_add(objetivoGlobal, pokemon);
}

t_entrenador* crearEntrenador(t_posicion* posicion, t_list* objetivos, t_list* pokemon, uint32_t cantObjetivos) {
    t_entrenador* entrenador = malloc(sizeof(t_entrenador));

    entrenador->id = getNuevoPid();
	entrenador->posicion = posicion;
    entrenador->pokemonCapturados = list_duplicate(pokemon);
    entrenador->pokemonObjetivo = list_duplicate(objetivos);
    entrenador->pokemonPlanificado = NULL;
	entrenador->entrenadorPlanificado = NULL;
	entrenador->cantidadObjetivo = cantObjetivos;
	entrenador->enEspera = false;
	entrenador->deadlock = entrenadorEnDeadlock(entrenador);
	entrenador->estimacionAnterior = config_get_int_value(config,"ESTIMACION_INICIAL");
	entrenador->cantCiclosCPU = 0;

	list_add(trIds,(void*)entrenador->id);

    return entrenador;
}

t_pokemon_posicion* crearPokemonPosicion(char* nombre, t_posicion* posicion){
	t_pokemon_posicion* pokemon = malloc(sizeof(t_pokemon_posicion));

	pokemon->nombre = nombre;
	pokemon->posicion = posicion;
	pokemon->tiempoCaptura = 1;

	return pokemon;
}

t_entrenador_posicion* crearEntrenadorPosicion(uint32_t id, t_posicion* posicion, t_entrenador* trainer) {
	t_entrenador_posicion* tr = malloc(sizeof(t_entrenador_posicion));

	tr->id = id;
	tr->posicion = posicion;
	tr->tiempoEjecucion = turnosHastaEntrenador(tr, trainer);
	tr->tiempoIntercambio = 5;

	return tr;
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
			if(strcmp(list_get(list1,i),list_get(list2,i)) != 0) {
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

bool entrenadorDisponible(void* entrenador) {
	t_entrenador* trainer = (t_entrenador*) entrenador;

	return (!entrenadorEnDeadlock(trainer) && !entrenadorCumplioObjetivo(trainer) && !trainer->enEspera);
}

t_entrenador* cambiarPosicionEntrenador(t_entrenador* entrenador, uint32_t posX, uint32_t posY) {
	entrenador->posicion->posicion_x = posX;
	entrenador->posicion->posicion_y = posY;
	return entrenador;
}

uint32_t getTiempoReal(t_entrenador* tr) {
	if(tr->entrenadorPlanificado != NULL) {
		// if(tr->entrenadorPlanificado->tiempoEjecucion != 0) {
			return tr->entrenadorPlanificado->tiempoEjecucion + tr->entrenadorPlanificado->tiempoIntercambio;
		// } else {
		// 	return tr->entrenadorPlanificado->tiempoIntercambio;
		// }
	} else if(tr->pokemonPlanificado != NULL) {
		return tr->pokemonPlanificado->tiempoEjecucion + tr->pokemonPlanificado->tiempoCaptura;
	}
	return ERROR;
}

uint32_t turnosHastaPokemon(t_pokemon_posicion* pokemon, t_entrenador* entrenador) {
	return abs(pokemon->posicion->posicion_x - entrenador->posicion->posicion_x) + abs(pokemon->posicion->posicion_y - entrenador->posicion->posicion_y);
}

uint32_t turnosHastaEntrenador(t_entrenador_posicion* tr, t_entrenador* entrenador) {
	return abs(tr->posicion->posicion_x - entrenador->posicion->posicion_x) + abs(tr->posicion->posicion_y - entrenador->posicion->posicion_y);
}

bool entrenadorLlegoASuDestino(void* entrenador) {
	t_entrenador* trainer = (t_entrenador*) entrenador;
	bool llegoAPoke = false;
	bool llegoAEntrenador = false;

	if(trainer->pokemonPlanificado != NULL) {
		llegoAPoke = trainer->pokemonPlanificado->tiempoCaptura == 0;
	} else {
		llegoAEntrenador = trainer->entrenadorPlanificado->tiempoIntercambio == 0;
	}

	return llegoAPoke || llegoAEntrenador;
}

void moverEntrenador(t_entrenador* entrenador){ 
    uint32_t entrenadorPosX = entrenador->posicion->posicion_x;
    uint32_t entrenadorPosY = entrenador->posicion->posicion_y; 

	uint32_t pokePosX; 
    uint32_t pokePosY; 
    
	if(entrenador->entrenadorPlanificado != NULL){
		pokePosX = entrenador->entrenadorPlanificado->posicion->posicion_x;
		pokePosY = entrenador->entrenadorPlanificado->posicion->posicion_y;
		entrenador->entrenadorPlanificado->tiempoEjecucion -= 1;
	} else if(entrenador->pokemonPlanificado != NULL){
		pokePosX = entrenador->pokemonPlanificado->posicion->posicion_x;
		pokePosY = entrenador->pokemonPlanificado->posicion->posicion_y;
		entrenador->pokemonPlanificado->tiempoEjecucion -= 1;
	}

    if((pokePosX - entrenadorPosX) == 0){
        if(pokePosY > entrenadorPosY)
            entrenador = cambiarPosicionEntrenador(entrenador, entrenadorPosX, entrenadorPosY+1);
        else
            entrenador = cambiarPosicionEntrenador(entrenador, entrenadorPosX, entrenadorPosY-1);
    }
    else {
        if(pokePosX > entrenadorPosX)
            entrenador = cambiarPosicionEntrenador(entrenador, entrenadorPosX+1, entrenadorPosY);
        else
            entrenador = cambiarPosicionEntrenador(entrenador, entrenadorPosX-1, entrenadorPosY);
    }
	log_info(logger, "El entrenador %i se movio a %i:%i \n", (uint32_t)entrenador->id, (uint32_t)entrenador->posicion->posicion_x, (uint32_t)entrenador->posicion->posicion_y);
	actualizarPosicion(entrenador);
}

void pasosParaIntercambio(t_entrenador* entrenador) {
	entrenador->entrenadorPlanificado->tiempoIntercambio -= 1;
	log_info(logger, "El entrenador %i puede realizar el intercambio en %i/5\n", (uint32_t)entrenador->id, (uint32_t)entrenador->entrenadorPlanificado->tiempoIntercambio+1);
	actualizarPosicion(entrenador);
}

void pasosParaCaptura(t_entrenador* entrenador) {
	entrenador->pokemonPlanificado->tiempoCaptura -= 1;
	log_info(logger, "El entrenador %i esta intentando capturar a %s\n", (uint32_t)entrenador->id, (uint32_t)entrenador->pokemonPlanificado->nombre);
	actualizarPosicion(entrenador);
}

void actualizarPosicion(t_entrenador* entrenador) {
	sem_wait(&mutexEXEC);
	list_remove(colaEXEC,0);
	list_add(colaEXEC, entrenador);
	sem_post(&mutexEXEC);
}

uint32_t getEstimacion(uint32_t estReal, uint32_t estAnterior) {
	double alpha = config_get_double_value(config,"ALPHA");

	return (estAnterior*alpha)+estReal*(1-alpha);
}

bool tardaMenos(void* trA, void* trB) {
	t_entrenador* trainerA = (t_entrenador*) trA;
	t_entrenador* trainerB = (t_entrenador*) trB;
	
	uint32_t cantTurnosA = getTiempoReal(trainerA);
	uint32_t cantTurnosB = getTiempoReal(trainerB);

	return getEstimacion(cantTurnosA, trainerA->estimacionAnterior) < getEstimacion(cantTurnosB, trainerB->estimacionAnterior);
}

bool pokemonEnObjetivoGlobal(t_pokemon_posicion* pokemon) {
    uint32_t index = pokemonPosicionPerteneceALista(pokemon->nombre,objetivoGlobal);
    if(index != ERROR) {
        if(((t_pokemon_cantidad*)list_get(objetivoGlobal,index))->cantidad > 0)
            return true;
    }
    return false;
}

bool esElMismoPoke(t_pokemon_posicion* poke, t_pokemon_posicion* poke2) {
	bool mismoNombre = strcmp(poke->nombre, poke2->nombre) == 0;
	bool mismaPosicionX = poke->posicion->posicion_x == poke2->posicion->posicion_x;
	bool mismaPosicionY = poke->posicion->posicion_y == poke2->posicion->posicion_y;

	return mismoNombre && mismaPosicionX && mismaPosicionY;
}

bool perteneceAEntrenador(t_list* list, t_pokemon_posicion* poke) {
	for(uint32_t i = 0; i < list_size(list); i++) {
		t_entrenador* tr = (t_entrenador*)list_get(list,i);
		if(tr->pokemonPlanificado != NULL) {
			if(esElMismoPoke(tr->pokemonPlanificado, poke)) {
				return true;
			}
		}
	}
	return false;
}

bool yaEstaAsignado(t_pokemon_posicion* poke) {
	sem_wait(&mutexBLOCKED);
	sem_wait(&mutexEXEC);
	bool perteneceABLOCKED = perteneceAEntrenador(colaBLOCKED, poke);
	bool perteneceAEXEC = perteneceAEntrenador(colaEXEC, poke);
	sem_post(&mutexEXEC);
	sem_post(&mutexBLOCKED);

	return perteneceABLOCKED || perteneceAEXEC;
}

void insertPokeEnMapa(t_pokemon_posicion* poke) {
	if(pokemonEnObjetivoGlobal(poke) && !yaEstaAsignado(poke)) {
		sem_wait(&mutexPokesEnMapa);
        list_add(pokemonesEnMapa, poke);
        sem_post(&mutexPokesEnMapa);
		sem_post(&counterPokesEnMapa);
	} else {
		free(poke);
	}
}

char* getMQName(uint32_t mq) {
	char* message;
	switch(mq) {
		case 2:
			message = "APPEARED_POKEMON";
			break;
		case 4:
			message = "CAUGHT_POKEMON";
			break;
		case 6:
			message = "LOCALIZED_POKEMON";
			break;
	}
	return message;
}

t_suscribe* getSuscribe(uint32_t mq) {
	t_suscribe* suscribe = malloc(sizeof(t_suscribe));
    uint32_t conexion = escuchaBroker();

	suscribe->conexion = conexion;
	suscribe->messageQueue = mq;

	return suscribe;
}

t_entrenador_catch* crearEntrenadorCatch(uint32_t mID, uint32_t trID) {
	t_entrenador_catch* entrenador = malloc(sizeof(t_entrenador_catch));

	entrenador->entrenadorID = trID;
	entrenador->mensajeId = mID;

	return entrenador;
}

t_entrenador_catch* getEntrenadorCatch(t_caught_pokemon* poke) {
	sem_wait(&mutexEntrenadoresCatch);
	t_list* esperandoCaught = list_duplicate(entrenadoresCatch);
	sem_post(&mutexEntrenadoresCatch);

	for(uint32_t i = 0; i < list_size(esperandoCaught); i++) {
		t_entrenador_catch* entrenador = (t_entrenador_catch*) list_get(esperandoCaught, i);
		if(entrenador->mensajeId == poke->ID_mensaje_original) {
			return entrenador;
		}
	}

	return NULL;
}

uint32_t getEntrenadorByID(uint32_t id, t_list* lista) {
	for(uint32_t i = 0; i < list_size(lista); i++) {
		t_entrenador* entrenador = list_get(lista, i);
		if(entrenador->id == id) {
			return i;
		}
	}
	return ERROR;
}

void resultadoParcial(t_list* lista) {
    for(uint32_t i = 0; i < list_size(lista); i++) {
        t_entrenador* tr = (t_entrenador*)list_get(lista,i);
        log_info(logger, "El entrenador %i capturo:", tr->id);
        for(uint32_t j = 0; j < list_size(tr->pokemonCapturados); j++) {
            log_info(logger, " - %s", list_get(tr->pokemonCapturados, j));
        }
		log_info(logger, "El entrenador %i necesita:", tr->id);
        for(uint32_t j = 0; j < list_size(tr->pokemonObjetivo); j++) {
            log_info(logger, " - %s", list_get(tr->pokemonObjetivo, j));
        }
    }
}

void procesarMensajeCaught(t_caught_pokemon* caughtPoke) {
	t_entrenador_catch* entrenador = getEntrenadorCatch(caughtPoke);
	if(entrenador && entrenador != NULL) {
		uint32_t index = getEntrenadorByID(entrenador->entrenadorID, colaBLOCKED);
		sem_wait(&mutexBLOCKED);
		t_entrenador* tr = (t_entrenador*)list_remove(colaBLOCKED,index);
		sem_post(&mutexBLOCKED);
		tr->enEspera = false;
		char* poke = malloc(strlen(tr->pokemonPlanificado->nombre)+1);
		memcpy(poke, tr->pokemonPlanificado->nombre, strlen(tr->pokemonPlanificado->nombre)+1);
		tr->pokemonPlanificado = NULL;
		if(caughtPoke->catchStatus == 1) {
			log_info(logger, "El entrenador %i pudo capturar a %s\n", tr->id, poke);
			list_add(tr->pokemonCapturados, poke);
			log_info(logger, "Ejecutando algoritmo de deteccion de deadlock");
			tr->deadlock = entrenadorEnDeadlock(tr);
			if(tr->deadlock) {
				log_info(logger, "El entrenador %i quedo en deadlock\n", tr->id);
				//sem_wait(&mutexBLOCKED);
				list_add(colaBLOCKED,tr);
				//sem_post(&mutexBLOCKED);
				detectorDeIntercambio();
			} else if(entrenadorCumplioObjetivo(tr)) {
				planificadorEXIT(tr);
			} else {
				//sem_wait(&mutexBLOCKED);
				list_add(colaBLOCKED,tr);
				sem_post(&counterEntrenadoresDisponibles);
				//sem_post(&mutexBLOCKED);
			}
		} else {
			actualizarObjetivoGlobal(poke, false);
			log_info(logger, "El entrenador %i no pudo capturar a %s\n", tr->id, poke);
			//sem_wait(&mutexBLOCKED);
			list_add(colaBLOCKED,tr);
			sem_post(&counterEntrenadoresDisponibles);
			//sem_post(&mutexBLOCKED);
		}
	}
}

t_list* obtenerCantPokes(t_list* lista){
	t_list* lista2 = list_create();
	t_list* listAux = list_create();
	for(int i = 0; i < list_size(lista); i++){
		char* poke = (char*)list_get(lista,i);
		if(!perteneceALista(poke, listAux)){
			uint32_t cantidad = perteneceAListaContador(poke, lista);
			list_add(lista2, setPokemonCantidad(poke,cantidad));
			list_add(listAux, poke);
		}
	}
	list_destroy(listAux);
	return lista2;
}


t_list* pokesQueNoQuiere(t_entrenador* tr) {
	t_list* mangaDeKokemones = list_create();
	t_list* pokesCapturados = obtenerCantPokes(tr->pokemonCapturados);
	for(uint32_t i = 0; i < list_size(pokesCapturados); i++) {
		t_pokemon_cantidad* pokeCant = (t_pokemon_cantidad*)list_get(pokesCapturados,i);
		uint32_t cantidad = perteneceAListaContador(pokeCant->nombre, tr->pokemonObjetivo);
		if(cantidad < pokeCant->cantidad){
			for(int j = 0; j < pokeCant->cantidad - cantidad; j++){
				list_add(mangaDeKokemones,pokeCant->nombre);
			}
		}else if(cantidad == 0){
			for(int s = 0; s < pokeCant->cantidad; s++){
				list_add(mangaDeKokemones,pokeCant->nombre);
			}
		}
	}
	return mangaDeKokemones;
}

t_list* pokesQueSiQuiere(t_entrenador* tr, t_list* pokes) {
	t_list* digimons = list_create();
	for(uint32_t i = 0; i < list_size(tr->pokemonObjetivo); i++) {
		if (perteneceALista((char*)list_get(tr->pokemonObjetivo, i), pokes)) {
			list_add(digimons, list_get(tr->pokemonObjetivo, i));
		}
	}
	return digimons;
}

uint32_t perteneceAListaInt(uint32_t n, t_list* list) {
	if(list_size(list) > 0) {
		for(uint32_t i=0; i < list_size(list); i++) {
			uint32_t num = (uint32_t)((t_trDeadlock*)list_get(list,i))->idEntrenador;
			if(n == num)
				return i;
		}
	}
	return ERROR;
}

uint32_t perteneceReporteDeadlock(t_entrenador* tr) {
	uint32_t result = ERROR;
	sem_wait(&mutexReporteDeadlock);
	t_list* auxDeadlock = list_duplicate(listaDeadlocks);
	sem_post(&mutexReporteDeadlock);
	if(list_size(auxDeadlock) > 0) {
		for(uint32_t i=0; i < list_size(auxDeadlock); i++) {
			t_deadlock* deadlock = list_get(auxDeadlock,i);
			if(!deadlock->estaResuelto) {
				if(perteneceAListaInt(tr->id, deadlock->entrenadores) != ERROR) {
					result = i;
					break;
				}
			}
		}
	}
	list_destroy(auxDeadlock);
	return result;
}

bool sigueEnDeadlock(t_deadlock* dl) {
	if(list_size(dl->entrenadores) > 0) {
		for(uint32_t i = 0; i < list_size(dl->entrenadores); i++) {
			t_trDeadlock* tr = (t_trDeadlock*)list_get(dl->entrenadores, i);
			if(tr->deadlock) {
				return true;
			} 
		}
	}
	return false;
}

t_trDeadlock* crearTrDeadlock(uint32_t idTr, bool deadlock) {
	t_trDeadlock* tr = malloc(sizeof(t_trDeadlock));

	tr->idEntrenador = idTr;
	tr->deadlock = deadlock;

	return tr;
}

t_trDeadlock* actualizarTrDeadlock(t_entrenador* tr, t_deadlock* dl) {
	uint32_t index = perteneceAListaInt(tr->id, dl->entrenadores);
	t_trDeadlock* trainer = (t_trDeadlock*)list_remove(dl->entrenadores, index);
	trainer->deadlock = entrenadorEnDeadlock(tr);
	return trainer;
}

t_deadlock* getReporteDeadlock(t_entrenador* tr) {
	uint32_t index = perteneceReporteDeadlock(tr);
	if(index != ERROR) {
		sem_wait(&mutexReporteDeadlock);
		t_deadlock* dl = (t_deadlock*)list_remove(listaDeadlocks,index);
		sem_post(&mutexReporteDeadlock);
		return dl;
	} else return NULL;
}

t_deadlock* crearReporteDeadlock() {
	t_deadlock* deadlock = malloc(sizeof(t_deadlock));

	deadlock->entrenadores = list_create();
	deadlock->cantIntercambios = 0;
	deadlock->estaResuelto = false;

	return deadlock;
}

void defaultCaptura(uint32_t index) {
	t_entrenador* tr = (t_entrenador*)list_get(colaBLOCKED,index);

	if(tr->enEspera && !tr->deadlock) {
		sem_wait(&mutexBLOCKED);
		list_remove(colaBLOCKED,index);
		sem_post(&mutexBLOCKED);
		tr->enEspera = false;
		char* poke = malloc(strlen(tr->pokemonPlanificado->nombre)+1);
		memcpy(poke, tr->pokemonPlanificado->nombre, strlen(tr->pokemonPlanificado->nombre)+1);
		log_info(logger, "MODO DEFAULT: El entrenador %i pudo capturar a %s\n", tr->id, poke);
		list_add(tr->pokemonCapturados, poke);
		log_info(logger, "Ejecutando algoritmo de deteccion de deadlock");
		tr->deadlock = entrenadorEnDeadlock(tr);
		tr->pokemonPlanificado = NULL;
		if(tr->deadlock) {
			log_info(logger, "El entrenador %i quedo en deadlock\n", tr->id);
			sem_wait(&mutexBLOCKED);
			list_add(colaBLOCKED,tr);
			sem_post(&mutexBLOCKED);
			detectorDeIntercambio();
		} else if(entrenadorCumplioObjetivo(tr)) {
			planificadorEXIT(tr);
		} else {
			sem_wait(&mutexBLOCKED);
			list_add(colaBLOCKED,tr);
			sem_post(&mutexBLOCKED);
			sem_post(&counterEntrenadoresDisponibles);
		}
	}
}