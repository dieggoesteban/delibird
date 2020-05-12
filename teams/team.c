#include "team.h"

t_log *logger;
t_config *config;

int main(void)
{
    config = config_create("./assets/team.config");

    IP = config_get_string_value(config, "IP_TEAM");
    PUERTO = config_get_string_value(config, "PUERTO_TEAM");
    LOG = config_get_string_value(config, "LOG_FILE");
    cicloCPU = (uint32_t)config_get_int_value(config, "RETARDO_CICLO_CPU");
    uint32_t idConexionPermanente;

    colaNEW = list_create();
    colaREADY = list_create();
    colaBLOCKED = list_create();
    colaEXEC = list_create();
    colaEXIT = list_create();

    pokemonesEnMapa = list_create();

    idConexionPermanente = escuchaBroker();
    t_register_module *registerCaught = malloc(sizeof(t_register_module));
    registerCaught->messageQueue = CAUGHT_POKEMON;
    t_paquete *paquete = serializar_registerModule(registerCaught);
    enviarMensaje(paquete, idConexionPermanente);

    uint32_t cod_op;
    if (recv(idConexionPermanente, &cod_op, sizeof(uint32_t), MSG_WAITALL) == -1)
        cod_op = -1;
    printf("\nCodigo de op: %i\n", cod_op);
    t_buffer *bufferRecibido = recibir_buffer(idConexionPermanente);
    // t_akc* akc = deserializar_akc(bufferRecibido);
    // printf("el akc es: %i", akc->AKC);

    t_caught_pokemon *caughtPoke = deserializar_caughtPokemon(bufferRecibido);

    printf("lo pudo agarrar? %i", caughtPoke->catchStatus);

    inicializarTeam();
    printf("idConexion: %i", idConexionPermanente);

    logger = log_create(LOG, "team", true, LOG_LEVEL_INFO);

    t_pokemon_posicion *pokemon = crearPokemonPosicion("Pitochu", crearPosicion(1, 1));

    t_posicion *pos1 = crearPosicion(4, 7);
    t_pokemon_posicion *poke1 = crearPokemonPosicion("Pikachu", pos1);
    t_posicion *pos2 = crearPosicion(5, 9);
    t_pokemon_posicion *poke2 = crearPokemonPosicion("Squirtle", pos2);
    t_posicion *pos3 = crearPosicion(8, 10);
    t_pokemon_posicion *poke3 = crearPokemonPosicion("Charmander", pos3);

    list_add(pokemonesEnMapa, poke3);
    list_add(pokemonesEnMapa, poke2);
    list_add(pokemonesEnMapa, poke1);
    list_add(pokemonesEnMapa, pokemon);

    //QUE PASA SI LLEGA UNO NUEVO CON NEW_POKEMON??
    while (list_size(pokemonesEnMapa) > 0)
    {
        if (!asignarPokemonAEntrenador())
            break;
    }

    for (uint32_t i = 0; i < list_size(colaREADY); i++)
    {
        printf("El entrenador asignado se encuentra en la posicion %i:%i \n", ((t_entrenador *)list_get(colaREADY, i))->posicion->posicion_x, ((t_entrenador *)list_get(colaREADY, i))->posicion->posicion_y);
    }

    // planificarFIFO();

    return 0;
}

void inicializarTeam()
{

    inicializarPid();
    inicializarEntrenadores();
    setObjetivoGlobal();
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