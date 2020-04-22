#include "team.h"

t_log* logger;
t_config* config;

int main(void)
{
    config = config_create("./assets/team.config");

    IP = config_get_string_value(config,"IP_TEAM");
    PUERTO = config_get_string_value(config,"PUERTO_TEAM");
    LOG = config_get_string_value(config,"LOG_FILE");

    t_list* entrenadores = inicializarEntrenadores();

    t_entrenador* entrenador = (t_entrenador *)list_get(entrenadores,0);

    t_list* lista = (t_list*)entrenador->pokemonCapturados;

    printf("soy del team %s\n", (char*)list_get(lista,0));

	logger = log_create(LOG,"team",true,LOG_LEVEL_INFO);
    // log_info(logger, "a ver");

    //iniciar_servidor();

    return 0;
}
