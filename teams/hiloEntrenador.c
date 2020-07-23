
#include "hiloEntrenador.h"

void cicloVidaEntrenador(t_entrenador* entrenador) {
    //ya se inicializaron los entrenadores y estan todos en new
    while(!entrenadorCumplioObjetivo(entrenador)) {
        sem_wait(&entrenador->mutex); //el primer wait espera a que le asignen un pokemon y pueda empezar.
        moverEntrenadorAPokemon(entrenador);
    }
    
}

//exec 

//actualizo la pos del entrenador

//se guarda en la colaEXEC

//se desaloja, se guarda en READY O BLOCKED --LA POSICION ES DIFERENTE