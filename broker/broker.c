#include "broker.h"

t_log* logger;
t_config* config;

int main(void)
{
    config = config_create("./assets/broker.config");

	logger = log_create("./assets/broker.log","broker",true,LOG_LEVEL_INFO);

    IP = config_get_string_value(config,"IP_BROKER");
    PUERTO = config_get_string_value(config,"PUERTO_BROKER");

    iniciar_servidor();

    return 0;
}
