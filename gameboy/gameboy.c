#include "gameboy.h"
#include <time.h>

t_log *logger;
t_log *gameboy_custom_logger;
t_config *config;

void finish()
{
    sem_post(&waitForFinish);
}

int main(int argc, char *argv[])
{   
    signal(SIGINT, finish); 
    
    sem_init(&waitForFinish, 0, 0);
    pthread_create(&t_finishProgram, NULL, (void*)terminarPrograma, NULL);
    pthread_detach(t_finishProgram);

    config = config_create("./assets/gameboy.config");
    ID_MODULE = (uint32_t)atoi(config_get_string_value(config, "ID_MODULO"));
    logger = log_create("./assets/gameboy.log", "gameboy", true, LOG_LEVEL_INFO);
    gameboy_custom_logger = log_create("./assets/gameboy_custom.log", "gameboy", false, LOG_LEVEL_DEBUG);

    ipBroker = config_get_string_value(config, "IP_BROKER");
    puertoBroker = config_get_string_value(config, "PUERTO_BROKER");

    if (argc > 2)
    {
        char *proceso = argv[1];
        char *tipo_mensaje = argv[2];
        t_list* listaArgumentos = cortarArgumentos(argc, argv);
        uint32_t isValid = procesarComando(&ipBroker, &puertoBroker, proceso, tipo_mensaje);

        if (isValid == 1)
        {
            uint32_t conexion = crear_conexion(ipBroker, puertoBroker);
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

                t_register_module* unsubscribe = crearSuscribe(suscribe->messageQueue, ID_MODULE);
                t_paquete* paquete = serializar_registerModule(unsubscribe, UNSUBSCRIBE);
                free(unsubscribe);
                enviarMensaje(paquete, crear_conexion(ipBroker, puertoBroker));
                close(suscribe->conexion);
                close(conexion);
                log_info(gameboy_custom_logger, "Conexion con broker finalizada");
            }
            else
            {
                paquete = getPaquete(listaArgumentos, tipo_mensaje);
                enviarMensaje(paquete, conexion);
                
                //serve_client(&conexion);
            }
            log_info(gameboy_custom_logger, "Conexion con broker liberada");
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

void terminarPrograma()
{
    sem_wait(&waitForFinish);
    
    t_register_module* unsubscribe = crearSuscribe(suscribe->messageQueue, ID_MODULE);
    t_paquete* paquete = serializar_registerModule(unsubscribe, UNSUBSCRIBE);
    free(unsubscribe);
    enviarMensaje(paquete, crear_conexion(ipBroker, puertoBroker));
    close(suscribe->conexion);
    log_info(gameboy_custom_logger, "Conexion con broker finalizada");

    exit(0);
}