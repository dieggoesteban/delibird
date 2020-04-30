#include "planificador.h"

t_entrenador *getEntrenadorMejorPosicionado(t_pokemon_posicion *pokemon)
{

    t_list* entrenadores = list_duplicate(colaNEW);
    t_list* blocked = list_filter(colaBLOCKED, entrenadorPuedeCapturar);
    printf("%i", list_size(blocked));
    list_add_all(entrenadores, blocked);

    t_entrenador *entrenadorMejorPosicionado = list_get(entrenadores, 0);

    uint32_t posX = pokemon->posicion->posicion_x;
    uint32_t posY = pokemon->posicion->posicion_y;

    uint32_t menorCantTurnos = abs(posX - entrenadorMejorPosicionado->posicion->posicion_x) + abs(posY - entrenadorMejorPosicionado->posicion->posicion_y);

    for (uint32_t i = 1; i < list_size(entrenadores); i++)
    {

        t_entrenador *trainer = list_get(entrenadores, i);

        uint32_t cantTurnos = abs(posX - trainer->posicion->posicion_x) + abs(posY - trainer->posicion->posicion_y);

        if (cantTurnos < menorCantTurnos)
        {
            entrenadorMejorPosicionado = trainer;

            menorCantTurnos = cantTurnos;
        }
        free(trainer);
    }

    list_destroy(entrenadores);

    return entrenadorMejorPosicionado;
}

//BOOL(uda)
void asignarPokemonAEntrenador(t_pokemon_posicion *pokemon)
{
    t_entrenador *entrenadorMP = getEntrenadorMejorPosicionado(pokemon);
    entrenadorMP->pokemonPlanificado = pokemon;

    uint32_t sePudo = moverEntrenadorDeCola(colaNEW, colaREADY, entrenadorMP);
    if (sePudo != 0)
    {
        moverEntrenadorDeCola(colaBLOCKED, colaREADY, entrenadorMP);
    }
}

//BOOL(uda)
uint32_t moverEntrenadorDeCola(t_list *colaEmisora, t_list *colaReceptora, t_entrenador *entrenador)
{
    uint32_t indexExiste = entrenadorPerteneceALista(entrenador, colaEmisora);
    if (indexExiste != -1)
    {
        list_add(colaReceptora, (t_entrenador *)list_remove(colaEmisora, indexExiste));
        return 0;
    }
    return 1;
}

