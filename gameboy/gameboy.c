#include "gameboy.h"

t_log* logger;
t_config* config;

int main(int argc, char* argv[])
{
    config = config_create("./assets/gameboy.config");

	logger = log_create("./assets/gameboy.log","gameboy",true,LOG_LEVEL_INFO);

    if(argc > 2) {
        char *proceso = argv[1];
	    char *tipo_mensaje = argv[2];
        char *ip;
	    char *puerto;
        char *arrayArgumentos[argc - 3];
        uint32_t conexion = crear_conexion(ip,puerto);
	    cortarArgumentos(argc, argv, arrayArgumentos);

        uint32_t isValid = procesarComando(&ip,&puerto,proceso,tipo_mensaje);
        
        if(isValid == 1) {
            log_info(logger,ip);
	        log_info(logger,puerto);
            t_paquete* paquete = crearPaquete(arrayArgumentos,tipo_mensaje);
            enviarMensaje(paquete,conexion);
        } else {
            printf("El comando no es valido para el proceso %s",proceso);
        }

    } else {
        printf("La cantidad de argumentos es invalida\n");
    }
    return 0;
}
