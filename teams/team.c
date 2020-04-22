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

	logger = log_create(LOG,"team",true,LOG_LEVEL_INFO);
    // log_info(logger, "a ver");

    //iniciar_servidor();

    return 0;
}
