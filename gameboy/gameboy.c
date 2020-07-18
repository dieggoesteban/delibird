#include "gameboy.h"
#include <time.h>

t_log *logger;
t_log *gameboy_custom_logger;
t_config *config;

int main(int argc, char *argv[])
{   
    srand(time(NULL));
    ID_MODULE = rand() % 40;
    config = config_create("./assets/gameboy.config");
    logger = log_create("./assets/gameboy.log", "gameboy", true, LOG_LEVEL_INFO);
    gameboy_custom_logger = log_create("./assets/gameboy_custom.log", "gameboy", true, LOG_LEVEL_INFO);

    ipBroker = config_get_string_value(config, "IP_BROKER");
    puertoBroker = config_get_string_value(config, "PUERTO_BROKER");

    if (argc > 2)
    {
        char *proceso = argv[1];
        char *tipo_mensaje = argv[2];
        char *ip;
        char *puerto;
        char *arrayArgumentos[argc - 3];
        cortarArgumentos(argc, argv, arrayArgumentos);

        uint32_t isValid = procesarComando(&ip, &puerto, proceso, tipo_mensaje);
        if (isValid == 1)
        {
            t_paquete *paquete;
            if (strcmp(proceso, "SUSCRIPTOR") == 0)
            {
                terminaConexion = false;
                temp = atoi(argv[3]);
                sem_init(&tempo, 0, 1);
                pthread_create(&hiloSuscriptor, NULL, (void*)modoSuscriptor, (void*)tipo_mensaje);
                pthread_create(&hiloTemporizador, NULL, (void*)temporizador, (void*)atoi(argv[3]));
                pthread_detach(hiloSuscriptor);
                pthread_join(hiloTemporizador, NULL);
            }
            else
            {
                uint32_t conexion = crear_conexion(ip, puerto);
                paquete = getPaquete(arrayArgumentos, tipo_mensaje);
                enviarMensaje(paquete, conexion);
                serve_client(&conexion);
            }          
        }
        else
        {
            log_error(gameboy_custom_logger, "Comando no valido para el proceso %s", proceso);
        }
    }
    else
    {
        log_error(gameboy_custom_logger, "La cantidad de argumentos es invalida");
    }
    return 0;
}
