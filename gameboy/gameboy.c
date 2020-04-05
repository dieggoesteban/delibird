#include "gameboy.h"

t_log* logger;
t_config* config;

int main(int argc, char* argv[])
{
    config = config_create("./assets/gameboy.config");

	logger = log_create("./assets/gameboy.log","gameboy",true,LOG_LEVEL_INFO);

    if(argc > 0) {
        procesarComando(argc,argv);
    } else {
        printf("No se paso ningun parametro");
    }
    return 0;
}
