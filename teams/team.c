#include "team.h"

t_log* logger;
t_config* config;

int main(void)
{
    config = config_create("./assets/team.config");

    IP = config_get_string_value(config,"IP_TEAM");
    PUERTO = config_get_string_value(config,"PUERTO_TEAM");
    LOG = config_get_string_value(config,"LOG_FILE");

    inicializarPid();

    t_list* entrenadores = inicializarEntrenadores();

    t_entrenador* entrenador0 = (t_entrenador *)list_get(entrenadores,0);
    t_entrenador* entrenador1 = (t_entrenador *)list_get(entrenadores,1);
    t_entrenador* entrenador2 = (t_entrenador *)list_get(entrenadores,2);


    printf("El entrenador 0 tiene el id: %i\n", (uint32_t)entrenador0->id);
    printf("El entrenador 1 tiene el id: %i\n", (uint32_t)entrenador1->id);
    printf("El entrenador 2 tiene el id: %i\n", (uint32_t)entrenador2->id);

    printf("El entrenador 0 puede capturar esta cantidad de pokemones: %i\n", (uint32_t)entrenador0->cantidadObjetivo);
    printf("El entrenador 1 puede capturar esta cantidad de pokemones: %i\n", (uint32_t)entrenador1->cantidadObjetivo);
    printf("El entrenador 2 puede capturar esta cantidad de pokemones: %i\n", (uint32_t)entrenador2->cantidadObjetivo);
    printf("El entrenador 2 tiene esta cantidad de pokes objetivos: %i\n", list_size(entrenador2->pokemonObjetivo));

    setObjetivoGlobal(entrenadores);
	
    logger = log_create(LOG,"team",true,LOG_LEVEL_INFO);
    // log_info(logger, "a ver");

    //iniciar_servidor();

    return 0;
}
