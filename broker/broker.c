#include "broker.h"

t_log* logger;
t_config* config;

int main(void)
{
    config = config_create("./assets/broker.config");

	logger = log_create("./assets/broker.log","broker",true,LOG_LEVEL_INFO);



    return 0;
}
