#include "gamecard.h"

t_log* logger;
t_config* config;

int main(void)
{
    config = config_create("./assets/gamecard.config");

	logger = log_create("./assets/gamecard.log","gamecard",true,LOG_LEVEL_INFO);
    log_info(logger, "a ver");

    IP = config_get_string_value(config,"IP_GAMECARD");
    PUERTO = config_get_string_value(config,"PUERTO_GAMECARD");

    iniciar_servidor();

    return 0;
}
