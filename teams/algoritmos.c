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

    return entrenador;
}

uint32_t getCurrentQuantum() {
    return --currentQuantum;
}

t_entrenador *RR(t_entrenador* e) {
    uint32_t q = getCurrentQuantum();
    if (q > 0) {
        return e;
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

AlgoritmoFunc getAlgoritmo(char* config) {
	if(strcmp(config,"FIFO") == 0) {
        desalojo = false;
		return FIFO;
	}
	else if (strcmp(config,"RR") == 0) {
        desalojo = true;
		return RR;
	}
	else if (strcmp(config,"SJF-CD") == 0) {
        desalojo = true;
		return SJF;
	}
	else if (strcmp(config,"SJF-SD") == 0) {
		desalojo = false;
		return SJF;
	}
    return NULL;
}