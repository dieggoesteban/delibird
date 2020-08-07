
#include "hiloEntrenador.h"

void hiloEntrenador(uint32_t trId) {
    //ya se inicializaron los entrenadores y estan todos en new
    uint32_t index = getEntrenadorByID(trId, colaNEW);
    sem_wait(&mutexNEW);
    t_entrenador* entrenador = list_get(colaNEW, index);
    sem_post(&mutexNEW);
    while(!entrenadorCumplioObjetivo(entrenador)) {
        sem_wait(&entrenador->mutex); //el primer wait espera a que le asignen un pokemon y pueda empezar.
        sem_wait(&mutexEXEC);
        entrenador = (t_entrenador*)list_get(colaEXEC, 0);
        sem_post(&mutexEXEC);
        entrenador->cantCiclosCPU += 1;
        if(entrenador->entrenadorPlanificado != NULL) {
            if(entrenador->entrenadorPlanificado->tiempoEjecucion == 0) {
                pasosParaIntercambio(entrenador);
            } else {
                moverEntrenador(entrenador);
            }
        } else {
            if(turnosHastaPokemon(entrenador->pokemonPlanificado,entrenador) == 0) {
                pasosParaCaptura(entrenador);
            } else {
                moverEntrenador(entrenador);
            }
        }
        sem_wait(&mutexEXEC);
        entrenador = (t_entrenador*)list_get(colaEXEC, 0);
        sem_post(&mutexEXEC);
    }
}

//exec 

//actualizo la pos del entrenador

//se guarda en la colaEXEC

//se desaloja, se guarda en READY O BLOCKED --LA POSICION ES DIFERENTE