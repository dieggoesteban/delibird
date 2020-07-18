#include "team.h"

t_log *logger;
t_config *config;

int main(void)
{
    config = config_create("./assets/team.config");
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

    suscribeCaught = getSuscribe(CAUGHT_POKEMON);
    suscribeAppeared = getSuscribe(APPEARED_POKEMON);
    suscribeLocalized = getSuscribe(LOCALIZED_POKEMON);

    sem_init(&mutexNEW, 0, 1);
    sem_init(&mutexREADY, 0, 1);
    sem_init(&mutexBLOCKED, 0, 1);
    sem_init(&mutexEXEC, 0, 1);
    sem_init(&mutexEXIT, 0, 1);
    sem_init(&mutexPokesEnMapa, 0, 1);
    sem_init(&counterPokesEnMapa, 0, 0);
    
    inicializarTeam();

    logger = log_create(LOG,"team",true,LOG_LEVEL_INFO);

    if (pthread_create(&threadREADY,NULL,(void*)planificadorREADY,NULL) != 0)
        printf("Error");

    if (pthread_create(&threadEXEC,NULL,(void*)planificadorEXEC,(void*)getAlgoritmo(ALGORITMO)) != 0)
        printf("Error");
    
    if(pthread_create(&threadSUSCRIBE_CAUGHT,NULL,(void*)suscribe,(void*)suscribeCaught) != 0)
        printf("Error");

    if(pthread_create(&threadSUSCRIBE_APPEARED,NULL,(void*)suscribe,(void*)suscribeAppeared) != 0)
        printf("Error");

    if(pthread_create(&threadSUSCRIBE_LOCALIZED,NULL,(void*)suscribe,(void*)suscribeLocalized) != 0)
        printf("Error");

    if (pthread_create(&threadSERVER,NULL,(void*)iniciar_servidor,NULL) != 0)
        printf("Error");

    signal(SIGTERM,terminar_programa);

    pthread_join(threadREADY, NULL);
    pthread_join(threadEXEC, NULL);
    pthread_join(threadSUSCRIBE_CAUGHT, NULL);
    pthread_join(threadSUSCRIBE_APPEARED, NULL);
    pthread_join(threadSUSCRIBE_LOCALIZED, NULL);
    pthread_join(threadSERVER, NULL);

    exit(0);
    return 0;
}

void terminar_programa() {
    printf("\nCERRANDO EL PROGRAMA*********************************");
    pthread_exit(&threadREADY);
    pthread_exit(&threadEXEC);
    pthread_exit(&threadSUSCRIBE_CAUGHT);
    pthread_exit(&threadSUSCRIBE_APPEARED);
    pthread_exit(&threadSUSCRIBE_LOCALIZED);
    pthread_exit(&threadSERVER);

    liberar_conexion(suscribeCaught->conexion);
    liberar_conexion(suscribeAppeared->conexion);
    liberar_conexion(suscribeLocalized->conexion);

    free(suscribeCaught);
    free(suscribeAppeared);
    free(suscribeLocalized);

    list_destroy(colaNEW);
    list_destroy(colaREADY);
    list_destroy(colaBLOCKED);
    list_destroy(colaEXEC);
    list_destroy(colaEXIT);
    list_destroy(pokemonesEnMapa);

    log_info(logger,"Liberamos todo");

    exit(0);
}

void inicializarTeam()
{

    inicializarPid();
    inicializarEntrenadores();
    setObjetivoGlobal();
    
    mandarGET();

    /*t_pokemon_posicion* pokemon = crearPokemonPosicion("Pitochu", crearPosicion(1,1));

    t_posicion* pos1 = crearPosicion(4,7);
    t_pokemon_posicion* poke1 = crearPokemonPosicion("Pikachu", pos1);
    t_posicion* pos2 = crearPosicion(5,9);
    t_pokemon_posicion* poke2 = crearPokemonPosicion("Squirtle", pos2);
    t_posicion* pos3 = crearPosicion(8,10);
    t_pokemon_posicion* poke3 = crearPokemonPosicion("Charmander", pos3);

    insertPokeEnMapa(poke1);
    insertPokeEnMapa(poke2);
    insertPokeEnMapa(poke3);
    insertPokeEnMapa(pokemon);*/

    //asignar pokes por posicion cercana LISTO

    //hacer los algoritmos de planificacion (FIFO) EN ESO
    //iniciar thread planificador

    //sendGET();
    //iniciar thread que compruebe que se conecto con broker o realizar la reconexion
    //iniciar threads conexiones con broker
    //comprobar si el mensaje es para el modulo
    //comprobar deadlock
    //hacer intercambio
    //finalizar
}