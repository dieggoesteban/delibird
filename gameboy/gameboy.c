#include "gameboy.h"

t_log* logger;
t_config* config;

int main(int argc, char* argv[])
{
    config = config_create("./assets/gameboy.config");

	logger = log_create("./assets/gameboy.log","gameboy",true,LOG_LEVEL_INFO);

    if(argc > 2) {
        char *proceso = argv[1];
        printf("Proceso: %s\n", argv[1]);
	    char *tipo_mensaje = argv[2];
        printf("Tipo Mensaje: %s\n", argv[2]);
        char *ip;
	    char *puerto;
        char *arrayArgumentos[argc - 3];
	    cortarArgumentos(argc, argv, arrayArgumentos);        

        uint32_t isValid = procesarComando(&ip,&puerto,proceso,tipo_mensaje);
        uint32_t conexion = crear_conexion(ip,puerto);
        if(isValid == 1) {
            log_info(logger,ip);
	        log_info(logger,puerto);
            t_paquete* paquete;
            if(strcmp(proceso, "SUSCRIBE") == 0)
            {
                paquete = modoSuscriptor(proceso);    
            }
            else
            {
                paquete = getPaquete(arrayArgumentos,tipo_mensaje);
            }        
            printf("el size del buffer es: %i", paquete->buffer->size);
            enviarMensaje(paquete,conexion);
        } else {
            printf("El comando no es valido para el proceso %s",proceso);
        }

    } else {
        printf("La cantidad de argumentos es invalida\n");
    }
    return 0;
}


/*

    TEAM 1 --> CATCH PIKACHU
    TEAM 2 --> CATCH PIKACHU

    BROKER --> RECIBO, PONGO ID Y ENVIAR EL ID QUE SE LE PUSO <-> ES DE TEAM

    BROKER --> GESTION DE IDs

    TEAM 1 Y 2 --> SE BLOQUEA EL HILO A LA ESPERA DE LA RESPUESTA CON EL ID ASIGNADO

*/