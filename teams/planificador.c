#include "planificador.h"

t_entrenador *getEntrenadorMejorPosicionado(t_pokemon_posicion *pokemon, t_list* entrenadores)
{

    t_entrenador *entrenadorMejorPosicionado = list_get(entrenadores, 0);

    // uint32_t posX = pokemon->posicion->posicion_x;
    // uint32_t posY = pokemon->posicion->posicion_y;

    uint32_t menorCantTurnos = turnosHastaPokemon(pokemon, entrenadorMejorPosicionado);
    for (uint32_t i = 1; i < list_size(entrenadores); i++) {
        t_entrenador *trainer = list_get(entrenadores, i);
        uint32_t cantTurnos = turnosHastaPokemon(pokemon, trainer);

        if (cantTurnos < menorCantTurnos) {
            entrenadorMejorPosicionado = trainer;
            menorCantTurnos = cantTurnos;
        }
    }

    return entrenadorMejorPosicionado;
}

bool asignarPokemonAEntrenador() {

    t_list* entrenadores = list_duplicate(colaNEW);
    t_list* blocked = list_filter(colaBLOCKED, entrenadorDisponible);
    list_add_all(entrenadores, blocked);
    bool resultado = false;

    if(list_size(pokemonesEnMapa) > 0) {
        t_pokemon_posicion *pokemon;
        while (list_size(pokemonesEnMapa) > 0) {
            pokemon = (t_pokemon_posicion*)list_remove(pokemonesEnMapa,0);
            if(pokemonEnObjetivoGlobal(pokemon)){
                break;
            } else {
                free(pokemon);
            }
        }
        if(pokemon != NULL) {
            if(list_size(entrenadores) > 0) {
                t_entrenador *entrenadorMP = getEntrenadorMejorPosicionado(pokemon, entrenadores);
                entrenadorMP->pokemonPlanificado = pokemon;
                bool sePudo = moverEntrenadorDeCola(colaNEW, colaREADY, entrenadorMP);
                if (!sePudo) {
                    moverEntrenadorDeCola(colaBLOCKED, colaREADY, entrenadorMP);
                }
                resultado = true;
            }
        }
    }

    list_destroy(blocked);
    list_destroy(entrenadores);

    return resultado;
}

t_entrenador* asignarAEntrenador(t_pokemon_posicion* pokemon) {

    sem_wait(&mutexNEW);
    t_list* entrenadores = list_duplicate(colaNEW);
    sem_post(&mutexNEW);
    sem_wait(&mutexBLOCKED);
    t_list* blocked = list_filter(colaBLOCKED, entrenadorDisponible);
    sem_post(&mutexBLOCKED);
    list_add_all(entrenadores, blocked);
    t_entrenador* entrenadorMP = NULL;

    if(pokemonEnObjetivoGlobal(pokemon) && list_size(entrenadores) > 0) {
        entrenadorMP = getEntrenadorMejorPosicionado(pokemon, entrenadores);
        entrenadorMP->pokemonPlanificado = pokemon;
    }

    list_destroy(blocked);
    list_destroy(entrenadores);

    return entrenadorMP;
}

//BOOL(uda)
bool moverEntrenadorDeCola(t_list *colaEmisora, t_list *colaReceptora, t_entrenador *entrenador) {
    uint32_t indexExiste = entrenadorPerteneceALista(entrenador, colaEmisora);
    if (indexExiste != ERROR) {
        //printf("**Posicion actual de entrenador %i:%i\n",(uint32_t)entrenador->posicion->posicion_x, (uint32_t)entrenador->posicion->posicion_y);
        list_add(colaReceptora, (t_entrenador *)list_remove(colaEmisora, indexExiste));
        return true;
    }
    return false;
}

void planificarFIFO(){ 
    t_queue* fifo = listToQueue(colaREADY); 
    while(queue_size(fifo) > 0){
        t_entrenador* entrenador = (t_entrenador*)queue_pop(fifo);

        ejecutarEntrenador(entrenador);
    }
}

// t_entrenador* FIFO(bool desalojo) {
//     t_entrenador* entrenador = list_take_and_remove(colaREADY, 0);
//     desalojo = false;

//     return entrenador;
// }

/*
void planificador(t_entrenador*(* plani)(bool)) {
    bool desalojo;
    t_entrenador* entrenador = plani(&desalojo);
    ejecutarEntrenador(desalojo, entrenador)

}

void ejecutarEntrenador {  
    if(desalojo) {
        moverEntrenador(entrenador);
        return;
    } else {
        while {
            moverEntrenador(entrenador);
        }
    }
}

*/
//LA FUNCION DE ASIGANR EL POKEMON MAS CERCANO AL FINAL CREO QUE RE DEBERIA SER como deciamos de que si esta al lado el pokemon que se agarre ese, no importa el "orden de llegada del pokemon", porque primero llegan todos los pokes con el get, y despues deberiamos empezar a asignar los entrenadores 
void ejecutarEntrenador(t_entrenador* entrenador){ //aunque no esta contemplando el tema del deadlock por ahora
    if(list_size(colaEXEC) == 0){ //solo puede haber uno ejecutando en colaEXEC
        list_add(colaEXEC, entrenador);
        uint32_t entrenadorPosX = entrenador->posicion->posicion_x; //las meto en variables porque es una re mil paja ver todo eso en el codigo
        uint32_t entrenadorPosY = entrenador->posicion->posicion_y; 
        uint32_t pokePosX = entrenador->pokemonPlanificado->posicion->posicion_x;
        uint32_t pokePosY = entrenador->pokemonPlanificado->posicion->posicion_y;
        uint32_t turnos = turnosHastaPokemon(entrenador->pokemonPlanificado, entrenador);

        printf("posicion inicial del entrenador: %i:%i\n", entrenadorPosX, entrenadorPosY);
        printf("posicion del poke asignado: %i:%i\n", pokePosX, pokePosY);
        printf("turnos hasta el poke: %i\n", turnos);
        for(uint32_t i = 0; i < turnos; i++ ){
            sleep(cicloCPU); //primero tarda los segundos que tiene que tardar y despues se le asigna la nueva posicion (habria que pensar que pasa si justo lo desaloja en la mitad del ciclo (que pasaria con la posicion del entrenador))
            //Los if de aca adentro siento que se re pueden sintetizar en una funcion aparte, but no me da la cabeza para eso ahora
            if((pokePosX - entrenadorPosX) == 0){ //si ya se movio todo lo que podia en eje x, empieza a moverse por eje y
                if(pokePosY > entrenadorPosY){ //si la posicion Y del pokemon es mayor a la del entrenador, significa que esta mas arriba, y por ende se le suma 1 a la posicion del entrenador
                    entrenador = cambiarPosicionEntrenador(entrenador, entrenadorPosX, entrenadorPosY+1);
                    entrenadorPosY++;
                }else{
                    entrenador = cambiarPosicionEntrenador(entrenador, entrenadorPosX, entrenadorPosY-1); //si la posicion Y del pokemon es menor a la del entrenador, significa que esta mas abajo, y por ende se le resta 1 a la posicion del entrenador
                    entrenadorPosY--;
                }
            }else{
                 if(entrenador->pokemonPlanificado->posicion->posicion_x > entrenadorPosX){ //si la posicion X del pokemon es mayor a la del entrenador, significa que esta mas arriba, y por ende se le suma 1 a la posicion del entrenador
                    entrenador = cambiarPosicionEntrenador(entrenador, entrenadorPosX+1, entrenadorPosY);
                    entrenadorPosX++;
                }else{
                    entrenador = cambiarPosicionEntrenador(entrenador, entrenadorPosX-1, entrenadorPosY); //si la posicion X del pokemon es menor a la del entrenador, significa que esta mas abajo, y por ende se le resta 1 a la posicion del entrenador
                    entrenadorPosX--;
                }
            }
            printf("la posicion del entrenador ahora es %i:%i\n", entrenadorPosX, entrenadorPosY);

        }   
        //SE MANDA EL CATCH
        
        mandarCATCH(entrenador);

        //SE PONE AL ENTRENADOR EN ESPERA
        entrenador->enEspera = true;
        moverEntrenadorDeCola(colaREADY,colaBLOCKED,entrenador);
        list_clean(colaEXEC); //hay que hacer las funciones aparte para liberar la memoria siempre que se haga list_destroy o clean (por las funciones de orden superior)
    }
}

void mandarCATCH(t_entrenador* entrenador) {
    
    // printf("hola antes de crear catch\n");
    t_catch_pokemon* catchPokemon = crearCatchPokemon(-1, entrenador->pokemonPlanificado->nombre, entrenador->pokemonPlanificado->posicion);
    // printf("hola antes de serializar\n");
    t_paquete* paquete = serializar_catchPokemon(catchPokemon);
    // printf("hola antes de deserializar\n");
    t_catch_pokemon* catchPokemon2 = deserializar_catchPokemon(paquete->buffer);

    printf("el nombre del poke deserializado: %s\n", catchPokemon2->nombre);


    free(catchPokemon);

    char* ip = config_get_string_value(config, "IP_BROKER");
    char* puerto = config_get_string_value(config, "PUERTO_BROKER");
    uint32_t conexion = crear_conexion(ip, puerto);

    enviarMensaje(paquete, conexion);

    liberar_conexion(conexion);

}

void* planificadorREADY() {
    printf("Hilo READY ejecutando\n");
    while(1) {
        printf("Esperando Pokemon...\n");
        sem_wait(&counterPokesEnMapa);
        sem_wait(&mutexPokesEnMapa);
        t_pokemon_posicion* poke = (t_pokemon_posicion*)list_remove(pokemonesEnMapa,0);
        printf("Saco a %s que esta en %i:%i \n",poke->nombre, (uint32_t)poke->posicion->posicion_x, (uint32_t)poke->posicion->posicion_y);
        sem_post(&mutexPokesEnMapa);
        t_entrenador* entrenador;
        do {
            entrenador = asignarAEntrenador(poke);
        }
        while(entrenador == NULL);
        bool sePudo = moverEntrenadorDeCola(colaNEW, colaREADY, entrenador);
        if (!sePudo) {
            moverEntrenadorDeCola(colaBLOCKED, colaREADY, entrenador);
        }
        printf("Asignado %s al entrenador %i que esta en %i:%i \n", poke->nombre, (uint32_t)entrenador->id, (uint32_t)entrenador->posicion->posicion_x, (uint32_t)entrenador->posicion->posicion_y);
    }
}

void* planificadorEXEC(void* arg) {
    AlgoritmoFunc* alg = (AlgoritmoFunc*) arg;
    printf("Hilo EXEC ejecutando\n");
    while(1) {
        if(list_size(colaREADY) > 0 || list_size(colaEXEC) > 0) {
            sleep(cicloCPU);
            t_entrenador* entrenador;
            //seria para el primer caso unicamente
            if(list_size(colaEXEC) == 0) {
                entrenador = alg(NULL);
                moverEntrenadorDeCola(colaREADY, colaEXEC, entrenador);
                sem_post(&entrenador->mutex);
                if(!entrenadorPuedeCapturar(entrenador)) {
                    moverEntrenadorAPokemon(entrenador);
                } else {
                    printf("Entrenador %i puede capturar y va a BLOCKED\n", entrenador->id);
                    entrenador->enEspera = true;
                    mandarCATCH(entrenador);
                    moverEntrenadorDeCola(colaEXEC, colaBLOCKED, entrenador);
                    if(list_size(colaREADY) > 0) {
                        t_entrenador* nuevoEntrenador = alg(NULL);
                        printf("Entrenador %i pasa a EXEC\n", nuevoEntrenador->id);
                        moverEntrenadorDeCola(colaREADY, colaEXEC, nuevoEntrenador);
                        moverEntrenadorAPokemon(nuevoEntrenador);
                    }
                }
            }
            //para todos los demas
            else {
                sem_wait(&mutexEXEC);
                entrenador = (t_entrenador*)list_get(colaEXEC,0);
                sem_post(&mutexEXEC);
                if(entrenadorPuedeCapturar(entrenador)) {
                    printf("Entrenador %i puede capturar y va a BLOCKED\n", entrenador->id);
                    entrenador->enEspera = true;
                    mandarCATCH(entrenador);
                    moverEntrenadorDeCola(colaEXEC, colaBLOCKED, entrenador);
                    if(list_size(colaREADY) > 0) {
                        t_entrenador* nuevoEntrenador = alg(NULL);
                        printf("Entrenador %i pasa a EXEC\n", nuevoEntrenador->id);
                        moverEntrenadorDeCola(colaREADY, colaEXEC, nuevoEntrenador);
                        moverEntrenadorAPokemon(nuevoEntrenador);
                    }
                }
                else if(desalojo && list_size(colaREADY) > 0) {
                    t_entrenador* nuevoEntrenador = alg(entrenador);
                    if(nuevoEntrenador->id != entrenador->id) {
                        printf("----Se desaloja a entrenador %i y entra entrenador %i\n", (uint32_t)entrenador->id, (uint32_t)nuevoEntrenador->id);
                        moverEntrenadorDeCola(colaEXEC, colaREADY, entrenador);
                        moverEntrenadorDeCola(colaREADY, colaEXEC, nuevoEntrenador);
                    }
                    moverEntrenadorAPokemon(nuevoEntrenador);
                }
                 else {
                    moverEntrenadorAPokemon(entrenador);
                }
            }
        }
    }
}