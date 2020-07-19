#include "planificador.h"

t_entrenador *getEntrenadorMejorPosicionado(t_pokemon_posicion *pokemon, t_list* entrenadores)
{

    t_entrenador *entrenadorMejorPosicionado = list_get(entrenadores, 0);

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

void mandarCATCH(t_entrenador* entrenador) {
    
    t_catch_pokemon* catchPokemon = crearCatchPokemon(-1, entrenador->pokemonPlanificado->nombre, entrenador->pokemonPlanificado->posicion);
    t_paquete* paquete = serializar_catchPokemon(catchPokemon);

    free(catchPokemon);

    char* ip = config_get_string_value(config, "IP_BROKER");
    char* puerto = config_get_string_value(config, "PUERTO_BROKER");
    uint32_t conexion = crear_conexion(ip, puerto);

    enviarMensaje(paquete, conexion);

    int cod_op;
	t_buffer* buffer;
	recv(conexion, &cod_op, sizeof(int), MSG_WAITALL);
	buffer = recibir_buffer(conexion);

    t_id_mensaje_recibido* confirmacion = deserializar_idMensajeRecibido(buffer);

    printf("El id de mensaje asignado es %i\n", (uint32_t)confirmacion->id_mensajeEnviado);

    sem_wait(&mutexEntrenadoresCatch);
    list_add(entrenadoresCatch, crearEntrenadorCatch(confirmacion->id_mensajeEnviado,entrenador->id));
    sem_post(&mutexEntrenadoresCatch);

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