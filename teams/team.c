#include "team.h"

t_log *logger;
t_config *config;

void handler() {
    sem_post(&waitForFinish);
}

int main(int argc, char *argv[])
{
    config = config_create("./assets/team.config");
    cantEntrenadores = 0;
    cantCambiosCtx = 0;
    cantDeadlocks = 0;
    cantIntercambios = 0;
    cantCiclosTotales = 0;
    primerRafaga = true;
    
    if(argv[1]) {
        idModule = atoi(argv[1]);
    }
    else {
        idModule = (uint32_t)config_get_int_value(config,"ID_MODULE");
    }

    signal(SIGINT, handler);

    printf("Id modulo: %i \n", idModule);
    logger = log_create("./assets/team.log", "team", true, LOG_LEVEL_INFO);
    IP = config_get_string_value(config,"IP_TEAM");
    PUERTO = config_get_string_value(config,"PUERTO_TEAM");
    LOG = config_get_string_value(config,"LOG_FILE");
    ALGORITMO = config_get_string_value(config,"ALGORITMO_PLANIFICACION");
    cicloCPU = (uint32_t)config_get_int_value(config,"RETARDO_CICLO_CPU");
    quantum = (uint32_t)config_get_int_value(config,"QUANTUM");
    currentQuantum = quantum;

    colaNEW = list_create();
    colaREADY = list_create();
    colaBLOCKED = list_create();
    colaEXEC = list_create();
    colaEXIT = list_create();
    pokemonesEnMapa = list_create();
    entrenadoresCatch = list_create();
    trIds = list_create();
    listaDeadlocks = list_create();

    sem_init(&mutexDetector, 0, 0);
    sem_init(&estaDesconectado, 0, 0);
    sem_init(&mutexNEW, 0, 1);
    sem_init(&mutexREADY, 0, 1);
    sem_init(&mutexBLOCKED, 0, 1);
    sem_init(&mutexEXEC, 0, 1);
    sem_init(&mutexPlanificadorEXEC, 0, 0);
    sem_init(&mutexEXIT, 0, 0);
    sem_init(&mutexPokesEnMapa, 0, 1);
    sem_init(&mutexReporteDeadlock, 0, 1);
    sem_init(&counterPokesEnMapa, 0, 0);
    sem_init(&counterEntrenadoresCatch, 0, 0);
    sem_init(&mutexReconnect, 0, 0);
    sem_init(&mutexEntrenadoresCatch, 0, 1);
    sem_init(&pokesObjetivoGlobal, 0, 1);
    sem_init(&waitForFinish, 0, 0);
    sem_init(&counterEntrenadoresDisponibles, 0, 0);
    
    inicializarTeam();
    establecerConexionBroker();
    sem_wait(&pokesObjetivoGlobal);
    mandarGET();
    sem_post(&pokesObjetivoGlobal);

    if (pthread_create(&finalizarPrograma,NULL,(void*)terminar_programa,NULL) != 0)
        printf("Error FINALIZAR\n");

    if (pthread_create(&threadREADY,NULL,(void*)planificadorREADY,NULL) != 0)
        printf("Error READY\n");

    if (pthread_create(&threadEXEC,NULL,(void*)planificadorEXEC,(void*)getAlgoritmo(ALGORITMO)) != 0)
        printf("Error EXEC\n");

    if (pthread_create(&threadSERVER,NULL,(void*)iniciar_servidor,NULL) != 0)
        printf("Error SERVIDOR\n");

    if (pthread_create(&threadDETECT_DISCON,NULL,(void*)detectarDesconexion,NULL) != 0)
        printf("Error DETECTOR DESCONEXION\n");

    if (pthread_create(&threadRECONNECT,NULL,(void*)reconectarBroker,NULL) != 0)
        printf("Error RECONEXION\n");

    pthread_detach(finalizarPrograma);
    pthread_join(threadEXIT, NULL);
    pthread_join(threadREADY, NULL);
    pthread_join(threadEXEC, NULL);
    pthread_join(threadDETECT_DEADLOCK, NULL);
    pthread_join(threadRECONNECT, NULL);
    pthread_join(threadSERVER, NULL);
    pthread_join(threadDETECT_DISCON, NULL);

    terminar_programa();
    return 0;
}

void terminar_programa() {
    sem_wait(&waitForFinish);
    printf("\nCERRANDO EL PROGRAMA*********************************\n");

    if(suscribeCaught->conexion != -1) {
        liberar_conexion(suscribeCaught->conexion);
    }
    if(suscribeAppeared->conexion != -1) {
        liberar_conexion(suscribeAppeared->conexion);
    }
    if(suscribeLocalized->conexion != -1) {
        liberar_conexion(suscribeLocalized->conexion);
    }

    desconectarBroker(CAUGHT_POKEMON);
    desconectarBroker(APPEARED_POKEMON);
    desconectarBroker(LOCALIZED_POKEMON);
    
    free(suscribeCaught);
    free(suscribeAppeared);
    free(suscribeLocalized);

    sem_destroy(&mutexNEW);
    sem_destroy(&mutexREADY);
    sem_destroy(&mutexBLOCKED);
    sem_destroy(&mutexEXEC);
    sem_destroy(&mutexEXIT);
    sem_destroy(&mutexPokesEnMapa);
    sem_destroy(&counterPokesEnMapa);

    list_destroy_and_destroy_elements(colaNEW, free);
    list_destroy_and_destroy_elements(colaBLOCKED, free);
    list_destroy_and_destroy_elements(colaEXEC, free);
    list_destroy_and_destroy_elements(colaEXIT, free);
    list_destroy_and_destroy_elements(pokemonesEnMapa, free);

    config_destroy(config);
    log_destroy(logger);

    exit(0);
}

void inicializarTeam()
{

    inicializarPid();
    inicializarEntrenadores();
    setObjetivoGlobal();
    pthread_t threadsEntrenadores[cantEntrenadores];

    for(uint32_t i = 0; i < cantEntrenadores; i++) {
        if (pthread_create(&threadsEntrenadores[i], NULL, (void *)hiloEntrenador, (void*)list_get(trIds, i)) != 0) {
          printf("No se puedo crear el hilo %i", i);
          break;
        }
    }

    for(uint32_t i = 0; i < cantEntrenadores; i++) {
        if (pthread_detach(threadsEntrenadores[i]) != 0) {
          printf("No se pudo hacer un detach del hilo %i\n", i);
        }
    }
    //asignar pokes por posicion cercana LISTO

    //hacer los algoritmos de planificacion EN ESO
    //iniciar thread planificador LISTO
    //sendGET(); LISTO
    //iniciar thread que compruebe que se conecto con broker o realizar la reconexion
    //iniciar threads conexiones con broker LISTO
    //comprobar si el mensaje es para el modulo
    //comprobar deadlock
    //hacer intercambio
    //finalizar LISTO
}