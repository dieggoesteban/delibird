
#include "hiloEntrenador.h"

void cicloVidaEntrenador(t_entrenador* entrenador) {
    //ya se inicializaron los entrenadores y estan todos en new
    while(!entrenadorCumplioObjetivo(entrenador)) {
        sem_wait(&entrenador->mutex); //el primer wait espera a que le asignen un pokemon y pueda empezar.
        moverEntrenadorAPokemon(entrenador);
    }
    
}