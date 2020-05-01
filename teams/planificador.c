#include "planificador.h"

t_entrenador *getEntrenadorMejorPosicionado(t_pokemon_posicion *pokemon, t_list* entrenadores)
{

    t_entrenador *entrenadorMejorPosicionado = list_get(entrenadores, 0);

    uint32_t posX = pokemon->posicion->posicion_x;
    uint32_t posY = pokemon->posicion->posicion_y;

    uint32_t menorCantTurnos = abs(posX - entrenadorMejorPosicionado->posicion->posicion_x) + abs(posY - entrenadorMejorPosicionado->posicion->posicion_y);

    for (uint32_t i = 1; i < list_size(entrenadores); i++) {
        t_entrenador *trainer = list_get(entrenadores, i);
        uint32_t cantTurnos = abs(posX - trainer->posicion->posicion_x) + abs(posY - trainer->posicion->posicion_y);

        if (cantTurnos < menorCantTurnos) {
            entrenadorMejorPosicionado = trainer;
            menorCantTurnos = cantTurnos;
        }
    }

    return entrenadorMejorPosicionado;
}

//BOOL(uda)
bool asignarPokemonAEntrenador() {

    t_list* entrenadores = list_duplicate(colaNEW);
    t_list* blocked = list_filter(colaBLOCKED, entrenadorPuedeCapturar);
    list_add_all(entrenadores, blocked);
    bool resultado = false;

    printf("hola\n");

    printf("pokes en el mapa %i\n", list_size(pokemonesEnMapa));

    if(list_size(pokemonesEnMapa) > 0) {
        t_pokemon_posicion *pokemon;
        while (list_size(pokemonesEnMapa) > 0) {
            pokemon = (t_pokemon_posicion*)list_remove(pokemonesEnMapa,0);
            if(pokemonEnObjetivoGlobal(pokemon)){
                printf("toy en el objetivo global\n");
                break;
            } else {
                printf("no toy en el objetivo global\n");
                free(pokemon);
            }
        }
        if(pokemon != NULL) {
            if(list_size(entrenadores) > 0) {
                t_entrenador *entrenadorMP = getEntrenadorMejorPosicionado(pokemon, entrenadores);
                entrenadorMP->pokemonPlanificado = pokemon;
                uint32_t sePudo = moverEntrenadorDeCola(colaNEW, colaREADY, entrenadorMP);
                if (sePudo != 0) {
                    moverEntrenadorDeCola(colaBLOCKED, colaREADY, entrenadorMP);
                }
                resultado = true;
            }
        } else {
            printf("no hay mas pokes que cumplan el obj global\n");
        }
    } else {
        printf("no hay mas pokes en el mapa\n");
    }

    list_destroy(blocked);
    list_destroy(entrenadores);

    return resultado;
}

//BOOL(uda)
uint32_t moverEntrenadorDeCola(t_list *colaEmisora, t_list *colaReceptora, t_entrenador *entrenador) {
    uint32_t indexExiste = entrenadorPerteneceALista(entrenador, colaEmisora);
    if (indexExiste != -1) {
        list_add(colaReceptora, (t_entrenador *)list_remove(colaEmisora, indexExiste));
        return 0;
    }
    return 1;
}

bool pokemonEnObjetivoGlobal(t_pokemon_posicion* pokemon) {
    uint32_t index = pokemonPosicionPerteneceALista(pokemon,objetivoGlobal);
    if(index != ERROR) {
        if(((t_pokemon_cantidad*)list_get(objetivoGlobal,index))->cantidad > 0)
            return true;
    }
    return false;
}

