#include "team.h"

t_log* logger;
t_config* config;

int main(void)
{
    config = config_create("./assets/team.config");

    IP = config_get_string_value(config,"IP_TEAM");
    PUERTO = config_get_string_value(config,"PUERTO_TEAM");
    LOG = config_get_string_value(config,"LOG_FILE");

    colaNEW = list_create();
    colaREADY = list_create();
    colaBLOCKED = list_create();

    inicializarTeam();
	
    logger = log_create(LOG,"team",true,LOG_LEVEL_INFO);

    t_pokemon_posicion* pokemon = malloc(sizeof(t_pokemon_posicion));

    pokemon->nombre = "Pitochu";
    pokemon->posicion = crearPosicion(1,1);

    t_entrenador* entrenadorMP = getEntrenadorMejorPosicionado(pokemon);

    printf("El entrenador mejor posicionado se encuentra en la posicion %i:%i\n", entrenadorMP->posicion->posicion_x, entrenadorMP->posicion->posicion_y);

    return 0;
}

void inicializarTeam() {

    inicializarPid();
    inicializarEntrenadores();
    setObjetivoGlobal();
    //asignar pokes por posicion cercana
    //hacer los algoritmos de planificacion (FIFO)
    //iniciar thread planificador

    //sendGET();
    //iniciar thread que compruebe que se conecto con broker o realizar la reconexion
    //iniciar threads conexiones con broker
    //comprobar si el mensaje es para el modulo
    //comprobar deadlock
    //hacer intercambio
    //finalizar

}