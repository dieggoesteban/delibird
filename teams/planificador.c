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
        entrenadorMP->pokemonPlanificado->tiempoEjecucion = turnosHastaPokemon(pokemon, entrenadorMP);
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

    log_info(logger, "El id de mensaje asignado es %i\n", (uint32_t)confirmacion->id_mensajeEnviado);

    sem_wait(&mutexEntrenadoresCatch);
    list_add(entrenadoresCatch, crearEntrenadorCatch(confirmacion->id_mensajeEnviado,entrenador->id));
    sem_post(&mutexEntrenadoresCatch);

    liberar_conexion(conexion);
}

void* planificadorREADY() {
    while(1) {
        sem_wait(&counterPokesEnMapa);
        sem_wait(&mutexPokesEnMapa);
        t_pokemon_posicion* poke = (t_pokemon_posicion*)list_remove(pokemonesEnMapa,0);
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
        sem_wait(&pokesObjetivoGlobal);
        actualizarObjetivoGlobal(poke->nombre, true);
        sem_post(&pokesObjetivoGlobal);
        log_info(logger, "Asignado %s al entrenador %i que esta en %i:%i. Pasa a READY", poke->nombre, (uint32_t)entrenador->id, (uint32_t)entrenador->posicion->posicion_x, (uint32_t)entrenador->posicion->posicion_y);
    }
}

void resultadosFinales() {
    for(uint32_t i = 0; i < list_size(colaEXIT); i++) {
        t_entrenador* tr = (t_entrenador*)list_get(colaEXIT,i);
        log_info(logger, "El entrenador %i capturo:", tr->id);
        for(uint32_t j = 0; j < list_size(tr->pokemonCapturados); j++) {
            log_info(logger, " - %s", list_get(tr->pokemonCapturados, 0));
        }
    }
}

void* planificadorEXIT() {
    while(1) {
        sem_wait(&mutexEXIT);
        sem_wait(&mutexBLOCKED);
        t_list* cumplioObjetivo = list_filter(colaBLOCKED, (void*)entrenadorCumplioObjetivo);
        sem_post(&mutexBLOCKED);
        if(list_size(cumplioObjetivo) > 1) {
            for(uint32_t i = 0; i < list_size(cumplioObjetivo); i++) {
                uint32_t index = getEntrenadorByID((uint32_t)((t_entrenador*)list_get(cumplioObjetivo,i))->id, colaBLOCKED);
                sem_wait(&mutexBLOCKED);
                t_entrenador* tr = (t_entrenador*)list_remove(colaBLOCKED, index);
                sem_post(&mutexBLOCKED);
                log_info(logger, "El entrenador %i completo su objetivo local. Pasa a EXIT\n", (uint32_t)tr->id);
                list_add(colaEXIT, tr);
            }
            if(list_size(colaEXIT) == cantEntrenadores) {
                log_info(logger, "EL MODULO %i COMPLETO EL OBJETIVO GLOBAL\n", idModule);
                break;
            }
        }
    }
    resultadosFinales();
    printf("\n\n────────▄███████████▄────────\n");
    printf("─────▄███▓▓▓▓▓▓▓▓▓▓▓███▄─────\n");
    printf("────███▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓███────\n");
    printf("───██▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓██───\n");
    printf("─██▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓██─\n");
    printf("██▓▓▓▓▓▓▓▓▓███████▓▓▓▓▓▓▓▓▓██\n");
    printf("██▓▓▓▓▓▓▓▓██░░░░░██▓▓▓▓▓▓▓▓██\n");
    printf("██▓▓▓▓▓▓▓██░░███░░██▓▓▓▓▓▓▓██\n");
    printf("███████████░░███░░███████████\n");
    printf("██░░░░░░░██░░███░░██░░░░░░░██\n");
    printf("██░░░░░░░░██░░░░░██░░░░░░░░██\n");
    printf("██░░░░░░░░░███████░░░░░░░░░██\n");
    printf("─██░░░░░░░░░░░░░░░░░░░░░░░██─\n");
    printf("───██░░░░░░░░░░░░░░░░░░░██───\n");
    printf("────███░░░░░░░░░░░░░░░███────\n");
    printf("─────▀███░░░░░░░░░░░███▀─────\n");
    printf("────────▀███████████▀────────\n");

    sem_post(&waitForFinish);
    return 0;
}

void* detectorDeIntercambio() {
    while(1) {
        sem_wait(&mutexDetector);
        sem_wait(&mutexBLOCKED);
        t_list* enDeadlock = list_filter(colaBLOCKED, (void*)entrenadorEnDeadlock);
        sem_post(&mutexBLOCKED);
        if(list_size(enDeadlock) > 1) {
            for(uint32_t i=0; i < list_size(enDeadlock); i++) {
                t_entrenador* tr1 = list_get(enDeadlock, i);
                for(uint32_t j=0; j < list_size(enDeadlock); j++) {
                    if(i != j) {
                        t_entrenador* tr2 = list_get(enDeadlock, j);
                        t_entrenador_posicion* trAsignado = getIntercambio(tr1, tr2);
                        if(trAsignado != NULL) {
                            log_info(logger, "Asignando al entrenador %i el entrenador %i\n", tr1->id, tr2->id);
                            tr1->entrenadorPlanificado = trAsignado;
                            uint32_t index = getEntrenadorByID(tr2->id, colaBLOCKED);
                            list_remove(colaBLOCKED, index);
                            tr2->enEspera = true;
                            sem_wait(&mutexBLOCKED);
                            list_add(colaBLOCKED,tr2);
                            sem_post(&mutexBLOCKED);
                            break;
                        }
                    }
                }
                if(tr1->entrenadorPlanificado != NULL) {
                    moverEntrenadorDeCola(colaBLOCKED, colaREADY, tr1);
                    break;
                }
            }
        }
        list_destroy(enDeadlock);   //---///        ////----///
    }
}

t_entrenador_posicion* getIntercambio(t_entrenador* tr1, t_entrenador* tr2) {
    t_list* bastardosDe1 = pokesQueNoQuiere(tr1);
    t_list* bastardosDe2 = pokesQueNoQuiere(tr2);
    t_list* pokesQueQuiere1 = pokesQueSiQuiere(tr1, bastardosDe2);

    t_entrenador_posicion* entrenador = NULL;

    bool ambosEstanDisponibles = !tr1->enEspera && !tr2->enEspera;

    if(list_size(pokesQueQuiere1) > 0 && ambosEstanDisponibles) {
        entrenador = (t_entrenador_posicion*)crearEntrenadorPosicion(tr2->id, tr2->posicion, tr1);
    }

    list_destroy(bastardosDe1);
    list_destroy(bastardosDe2);
    list_destroy(pokesQueQuiere1);

    return entrenador;
}

void realizarIntercambio(t_entrenador* tr) {
    t_list* bastardosDe1 = pokesQueNoQuiere(tr);
    uint32_t index2 = getEntrenadorByID(tr->entrenadorPlanificado->id, colaBLOCKED);
    sem_wait(&mutexBLOCKED);
    t_entrenador* tr2 = list_get(colaBLOCKED, index2);
    sem_post(&mutexBLOCKED);
    t_list* bastardosDe2 = pokesQueNoQuiere(tr2);

    t_list* pokesQueQuiere1 = pokesQueSiQuiere(tr, bastardosDe2);
    t_list* pokesQueQuiere2 = pokesQueSiQuiere(tr2, bastardosDe1);

    if((list_size(pokesQueQuiere1) > 0)) {
        char* poke1 = (char*)list_get(pokesQueQuiere1,0);
        char* poke2;
        if(list_size(pokesQueQuiere2) > 0) {
            poke2 = (char*)list_get(pokesQueQuiere2,0);
        } else {
            poke2 = (char*)list_get(bastardosDe1, 0);
        }
        log_info(logger, "Realizando el intercambio entre el entrenador %i (%s) y el entrenador %i (%s)", tr->id, poke1, tr2->id, poke2);
        list_remove(tr->pokemonCapturados, getIndexList(poke2, tr->pokemonCapturados));
        list_remove(tr2->pokemonCapturados, getIndexList(poke1, tr2->pokemonCapturados));
        list_add(tr->pokemonCapturados, poke1);
        list_add(tr2->pokemonCapturados, poke2);
    } else {
        log_info(logger, "Hubo un error en el intercambio\n");
    }
    mostrarPokesTrainer(tr);
    mostrarPokesTrainer(tr2);
    tr->entrenadorPlanificado = NULL;
    tr->enEspera = false;
    tr2->enEspera = false;
    tr->deadlock = entrenadorEnDeadlock(tr);
    if(tr->deadlock) {
        log_info(logger, "El entrenador %i quedo en deadlock\n", tr->id);
        sem_post(&mutexDetector);
    }
    tr2->deadlock = entrenadorEnDeadlock(tr2);
    if(tr2->deadlock) {
        log_info(logger, "El entrenador %i quedo en deadlock\n", tr2->id);
        sem_post(&mutexDetector);
    }

    sem_wait(&mutexBLOCKED);
    moverEntrenadorDeCola(colaBLOCKED, colaBLOCKED, tr2);
    sem_post(&mutexBLOCKED);
    moverEntrenadorDeCola(colaEXEC, colaBLOCKED, tr);
    sem_post(&mutexEXIT);
}


void modoDesconectado() {
    sem_wait(&mutexBLOCKED);
    if(list_size(colaBLOCKED) > 0) {
        for(uint32_t i = 0; i < list_size(colaBLOCKED); i++) {
            defaultCaptura(i);
        }
    }
    sem_post(&mutexBLOCKED);
    sem_post(&mutexEXIT);
}

void* planificadorEXEC(void* arg) {
    AlgoritmoFunc* alg = (AlgoritmoFunc*) arg;
    while(1) {
        if(list_size(colaREADY) > 0 || list_size(colaEXEC) > 0) {
            sleep(cicloCPU);
            t_entrenador* entrenador;
            //seria para el primer caso unicamente
            if(list_size(colaEXEC) == 0) {
                entrenador = alg(NULL);
                moverEntrenadorDeCola(colaREADY, colaEXEC, entrenador);
                if(!entrenadorLlegoASuDestino(entrenador)) {
                    sem_post(&entrenador->mutex);
                } else {
                    if(entrenador->pokemonPlanificado != NULL) {
                        log_info(logger, "El entrenador %i puede capturar y va a BLOCKED", entrenador->id);
                        entrenador->enEspera = true;
                        int valorSem;
                        if (sem_getvalue(&estaDesconectado, &valorSem) == 0){
                            if(valorSem == 0){
                                mandarCATCH(entrenador);
                                moverEntrenadorDeCola(colaEXEC, colaBLOCKED, entrenador);
                            } else {
                                moverEntrenadorDeCola(colaEXEC, colaBLOCKED, entrenador);
                                modoDesconectado();
                            }
                        }
                    }
                    else if(entrenador->entrenadorPlanificado != NULL) {
                        realizarIntercambio(entrenador);
                    }
                    if(list_size(colaREADY) > 0) {
                        t_entrenador* nuevoEntrenador = alg(NULL);
                        log_info(logger, "El entrenador %i pasa a EXEC", nuevoEntrenador->id);
                        moverEntrenadorDeCola(colaREADY, colaEXEC, nuevoEntrenador);
                        sem_post(&nuevoEntrenador->mutex);
                    }
                }
            }
            //para todos los demas
            else {
                sem_wait(&mutexEXEC);
                entrenador = (t_entrenador*)list_get(colaEXEC,0);
                sem_post(&mutexEXEC);
                if(entrenadorLlegoASuDestino(entrenador)) {
                    if(entrenador->pokemonPlanificado != NULL) {
                        log_info(logger, "El entrenador %i puede capturar y va a BLOCKED", entrenador->id);
                        entrenador->enEspera = true;
                        int valorSem;
                        if (sem_getvalue(&estaDesconectado, &valorSem) == 0){
                            if(valorSem == 0){
                                mandarCATCH(entrenador);
                                moverEntrenadorDeCola(colaEXEC, colaBLOCKED, entrenador);
                            } else {
                                moverEntrenadorDeCola(colaEXEC, colaBLOCKED, entrenador);
                                modoDesconectado();
                            }
                        }
                    }
                    else if(entrenador->entrenadorPlanificado != NULL) {
                        realizarIntercambio(entrenador);
                    }
                    if(list_size(colaREADY) > 0) {
                        t_entrenador* nuevoEntrenador = alg(NULL);
                        log_info(logger, "El entrenador %i pasa a EXEC", nuevoEntrenador->id);
                        moverEntrenadorDeCola(colaREADY, colaEXEC, nuevoEntrenador);
                        sem_post(&nuevoEntrenador->mutex);
                    }
                }
                else if(desalojo && list_size(colaREADY) > 0) {
                    t_entrenador* nuevoEntrenador = alg(entrenador);
                    if(nuevoEntrenador->id != entrenador->id) {
                        log_info(logger, "Se desaloja de EXEC al entrenador %i y entra entrenador %i", entrenador->id, nuevoEntrenador->id);
                        moverEntrenadorDeCola(colaEXEC, colaREADY, entrenador);
                        moverEntrenadorDeCola(colaREADY, colaEXEC, nuevoEntrenador);
                    }
                    sem_post(&nuevoEntrenador->mutex);
                }
                 else {
                    sem_post(&entrenador->mutex);
                }
            }
        }
    }
}