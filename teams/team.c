#include "team.h"

t_log* logger;
t_config* config;

int main(void)
{
    config = config_create("./assets/team.config");

    IP = config_get_string_value(config,"IP_TEAM");
    PUERTO = config_get_string_value(config,"PUERTO_TEAM");
    LOG = config_get_string_value(config,"LOG_FILE");
    ALGORITMO = config_get_string_value(config,"ALGORITMO_PLANIFICACION");
    cicloCPU = (uint32_t)config_get_int_value(config,"RETARDO_CICLO_CPU");
    quantum = (uint32_t)config_get_int_value(config,"QUANTUM");
    currentQuantum = quantum;

    pthread_t threadREADY;
    pthread_t threadEXEC;
    pthread_t threadSERVER;

    colaNEW = list_create();
    colaREADY = list_create();
    colaBLOCKED = list_create();
    colaEXEC = list_create();
    colaEXIT = list_create();
    pokemonesEnMapa = list_create();

    sem_init(&mutexNEW, 0, 1);
    sem_init(&mutexREADY, 0, 1);
    sem_init(&mutexBLOCKED, 0, 1);
    sem_init(&mutexEXEC, 0, 1);
    sem_init(&mutexEXIT, 0, 1);
    sem_init(&mutexPokesEnMapa, 0, 1);
    sem_init(&counterPokesEnMapa, 0, 0);
    
    inicializarTeam();
	
    logger = log_create(LOG,"team",true,LOG_LEVEL_INFO);
    

    // while(list_size(pokemonesEnMapa) > 0) {
    //     if (!asignarPokemonAEntrenador())
    //         break;
    // }

    // for(uint32_t i = 0; i < list_size(colaREADY); i++) {
    //     printf("El entrenador asignado se encuentra en la posicion %i:%i \n", ((t_entrenador*)list_get(colaREADY,i))->posicion->posicion_x, ((t_entrenador*)list_get(colaREADY,i))->posicion->posicion_y);
    // }    
    // planificarFIFO();

    if (pthread_create(&threadREADY,NULL,(void*)planificadorREADY,NULL) != 0)
        printf("Error");

    if (pthread_create(&threadEXEC,NULL,(void*)planificadorEXEC,(void*)getAlgoritmo(ALGORITMO)) != 0)
        printf("Error");

    if (pthread_create(&threadSERVER, NULL,(void*)iniciar_servidor,NULL) != 0)
        printf("Error");

    pthread_join(threadREADY, NULL);
    pthread_join(threadEXEC, NULL);
    pthread_join(threadSERVER, NULL);

    return 0;
}

void inicializarTeam() {

    inicializarPid();
    inicializarEntrenadores();
    setObjetivoGlobal();

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