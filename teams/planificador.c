#include "planificador.h"

void inicializarPid() {
    pid = 0;
}

uint32_t getNuevoPid() {
    uint32_t nuevoPid = pid++;

    return nuevoPid;
}

t_entrenador* getEntrenadorMejorPosicionado(t_pokemon_posicion* pokemon) {

    t_list* entrenadores = list_duplicate(colaNEW);

    t_entrenador* entrenadorMejorPosicionado = list_get(entrenadores, 0);

    uint32_t posX = pokemon->posicion->posicion_x;
    uint32_t posY = pokemon->posicion->posicion_y;

    uint32_t menorCantTurnos = abs(posX - entrenadorMejorPosicionado->posicion->posicion_x) + abs(posY - entrenadorMejorPosicionado->posicion->posicion_y);

    for(uint32_t i = 1; i < list_size(entrenadores); i++) {

        t_entrenador* trainer = list_get(entrenadores,i);
        uint32_t cantTurnos = abs(posX - trainer->posicion->posicion_x)+abs(posY - trainer->posicion->posicion_y);

        if(cantTurnos < menorCantTurnos) {
            entrenadorMejorPosicionado = trainer;

            menorCantTurnos = cantTurnos;
        }
        free(trainer);
    }

    list_destroy(entrenadores);

    return entrenadorMejorPosicionado;

}