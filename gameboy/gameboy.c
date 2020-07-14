#include "gameboy.h"

t_log *logger;
t_log *gameboy_custom_logger;
t_config *config;

int main(int argc, char *argv[])
{
    config = config_create("./assets/gameboy.config");
    logger = log_create("./assets/gameboy.log", "gameboy", true, LOG_LEVEL_INFO);
    gameboy_custom_logger = log_create("./assets/gameboy_custom.log", "gameboy", true, LOG_LEVEL_INFO);

    if (argc > 2)
    {
        char *proceso = argv[1];
        log_info(gameboy_custom_logger, "Proceso: %s", argv[1]);
        char *tipo_mensaje = argv[2];
        log_info(gameboy_custom_logger, "Tipo mensaje: %s", argv[2]);
        char *ip;
        char *puerto;
        char *arrayArgumentos[argc - 3];
        cortarArgumentos(argc, argv, arrayArgumentos);

        uint32_t isValid = procesarComando(&ip, &puerto, proceso, tipo_mensaje);
        uint32_t conexion = crear_conexion(ip, puerto);
        if (isValid == 1)
        {
            t_paquete *paquete;
            if (strcmp(proceso, "SUSCRIPTOR") == 0)
            {
                if(strcmp(tipo_mensaje, "NEW_POKEMON") == 0)               
                    paquete = modoSuscriptor(1);
                else if (strcmp(tipo_mensaje, "APPEARED_POKEMON") == 0) 
                    paquete = modoSuscriptor(2);
                else if (strcmp(tipo_mensaje, "CATCH_POKEMON") == 0) 
                    paquete = modoSuscriptor(3);
                else if (strcmp(tipo_mensaje, "CAUGHT_POKEMON") == 0) 
                    paquete = modoSuscriptor(4);
                else if (strcmp(tipo_mensaje, "GET_POKEMON") == 0) 
                    paquete = modoSuscriptor(5);
                else if (strcmp(tipo_mensaje, "LOCALIZED_POKEMON") == 0) 
                    paquete = modoSuscriptor(6);
                enviarMensaje(paquete, conexion);
                while(true) //TODO: Aca hay que ver como hacer lo del tiempo con el parametro
                {
                    serve_client(&conexion);
                }
            }
            else
            {
                paquete = getPaquete(arrayArgumentos, tipo_mensaje);
                enviarMensaje(paquete, conexion);
                while(true)
                {
                    serve_client(&conexion); //TODO: Hay que hacer que salga del while cuando recibe el ID del mensaje
                }
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