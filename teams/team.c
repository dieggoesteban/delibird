#include "team.h"

t_log* logger;
t_config* config;

int main(void)
{
    config = config_create("./assets/team.config");

	logger = log_create("./assets/team.log","team",true,LOG_LEVEL_INFO);
    log_info(logger, "a ver");

    IP = config_get_string_value(config,"IP_TEAM");
    PUERTO = config_get_string_value(config,"PUERTO_TEAM");

    iniciar_servidor();

    return 0;
}
