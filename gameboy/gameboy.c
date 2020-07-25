#include "gameboy.h"
#include <time.h>

t_log *logger;
t_log *gameboy_custom_logger;
t_config *config;

int main(int argc, char *argv[])
{   
    config = config_create("./assets/gameboy.config");
    logger = log_create("./assets/gameboy.log", "gameboy", true, LOG_LEVEL_INFO);
    gameboy_custom_logger = log_create("./assets/gameboy_custom.log", "gameboy", true, LOG_LEVEL_INFO);
    ID_MODULE = (uint32_t)atoi(config_get_string_value(config, "ID_MODULO"));

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
            uint32_t conexion = crear_conexion(ip, puerto);
            t_paquete *paquete;
            if (strcmp(proceso, "SUSCRIPTOR") == 0)
            {
                uint32_t mq = getColaDeMensajes(tipo_mensaje);
                temp = atoi(argv[3]);
                t_suscribe_gameboy* suscribe = crearSuscribeGameboy(conexion,mq);

                pthread_create(&hiloSuscriptor, NULL, (void*)modoSuscriptor, (void*)suscribe);
                pthread_create(&hiloTemporizador, NULL, (void*)temporizador, (void*)temp);
                pthread_detach(hiloSuscriptor);
                pthread_join(hiloTemporizador, NULL);
                log_info(logger, "Conexion con broker finalizada");
            }
            else
            {
                paquete = getPaquete(arrayArgumentos, tipo_mensaje);
                enviarMensaje(paquete, conexion);
                
                //serve_client(&conexion);
            }
            printf("Liberamos conexion\n");
            liberar_conexion(conexion);
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