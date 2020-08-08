#include "algoritmos.h"

t_entrenador *FIFO(t_entrenador* e) {
    sem_wait(&mutexREADY);
    t_entrenador* entrenador = list_get(colaREADY, 0);
    sem_post(&mutexREADY);

    return entrenador;
}

t_entrenador *SJF(t_entrenador* e) {
    sem_wait(&mutexREADY);
    t_list* ready = list_duplicate(colaREADY);
    sem_post(&mutexREADY);
    if(e != NULL)
        list_add(ready, e);
    
    list_sort(ready,tardaMenos);
    t_entrenador* entrenador = list_get(ready, 0);

    entrenador->estimacionAnterior = getEstimacion(getTiempoReal(entrenador), entrenador->estimacionAnterior);

    moverEntrenadorDeCola(colaREADY, colaREADY, entrenador);

    return entrenador;
}

uint32_t getCurrentQuantum() {
    return --currentQuantum;
}

t_entrenador *RR(t_entrenador* e) {
    uint32_t q = getCurrentQuantum();
    if(primerRafaga) {
        currentQuantum = quantum;
        primerRafaga = false;
    }
    //printf("quantum actual: %i\n", q);
    if (q > 0) {
        if(e != NULL) {
            return e;
        } else {
            sem_wait(&mutexREADY);
            t_entrenador* entrenador = list_get(colaREADY, 0);
            sem_post(&mutexREADY);

            return entrenador;
        }
    }
    else {
        currentQuantum = quantum;
        sem_wait(&mutexREADY);
        t_entrenador* entrenador = list_get(colaREADY, 0);
        sem_post(&mutexREADY);

        return entrenador;
    }
}

//FIFO/RR/SJF-CD/SJF-SD

AlgoritmoFunc* getAlgoritmo(char* config) {
	if(strcmp(config,"FIFO") == 0) {
        desalojo = false;
        log_info(logger, "Algoritmo empleado: FIFO");
		return FIFO;
	}
	else if (strcmp(config,"RR") == 0) {
        desalojo = true;
        log_info(logger, "Algoritmo empleado: Round Robin");
		return RR;
	}
	else if (strcmp(config,"SJF-CD") == 0) {
        desalojo = true;
        log_info(logger, "Algoritmo empleado: SJF Con Desalojo");
		return SJF;
	}
	else if (strcmp(config,"SJF-SD") == 0) {
		desalojo = false;
        log_info(logger, "Algoritmo empleado: SJF Sin Desalojo");
		return SJF;
	}
    return NULL;
}