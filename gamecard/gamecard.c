#include "gamecard.h"


int main(void)
{
    config = config_create("./assets/gamecard.config");

	logger = log_create("./assets/gamecard.log","gamecard",true,LOG_LEVEL_INFO);

    log_info(logger, "a ver");

    uint32_t idConexionPermanente = escuchaBroker();
    pthread_t hiloEscuchaBroker;
    pthread_t hiloEscuchaGameboy;

    inicializarGamecard();
  

    
    // t_posicion_cantidad* posCantPikachu = crearPosicionCantidad(1,5,20);
    // t_new_pokemon* pikachu = crearNewPokemon(1,"Pikachu",posCantPikachu);

    iniciar_servidor();


    // atenderNewPokemon(pikachu);


    // sem_init(&semReconexion,0,1);

    // pthread_create(&hiloEscuchaBroker, NULL, escuchaPermanenteBroker,&idConexionPermanente);
    // pthread_join(hiloEscuchaBroker,NULL);

    //CADA VEZ QUE HAGAMOS UN RECV HAY QUE CHEQUEAR QUE NO DE -1, Y SI LO DA, SE LLAMA A RECONEXIONBROKER
    //podriamos hacer una funcion protocolo que llame a reconexion broker, el cual devuelve el nuevo int conexion y luego volver a llamar a escucharPermanenteBroker
    //quizas deberia hacer un pthread_join de hiloEscucha primero, para poder crear un nuevo hilo con ese struct.

    return 0;
}

void* escuchaPermanenteBroker(void* idConexionPermanente){
    uint32_t* idCon = (uint32_t*)idConexionPermanente;
    // log_info(logger,"idCOnecsdcPermanente: %i", (uint32_t)idConexionPermanente);
    uint32_t cod_op;
    while(1){
        int result = recv((uint32_t)idConexionPermanente, &cod_op, sizeof(uint32_t), MSG_WAITALL);
        if(result == ERROR){
            sem_wait(&semReconexion);
            pthread_t hiloReconexion;
            log_info(logger,"a punto de entrar a rec");
            pthread_create(&hiloReconexion,NULL,reconexionBroker,NULL);
            // uint32_t idNuevaConexion;
            pthread_join(hiloReconexion,(void**)&idCon);
            
  
            
        }
    }
}

void* reconexionBroker(){
    log_info(logger,"entro a rec");
    uint32_t idCon = -1;
    while(idCon == ERROR){
        idCon = escuchaBroker();
        log_info(logger,"idConexion en reintento: %i", idCon);
        sleep(tiempoReintentoConexion);
    }
    pthread_exit((void*)idCon);
    sem_post(&semReconexion);


}

void inicializarGamecard(){
    IP = config_get_string_value(config,"IP_GAMECARD");
    PUERTO = config_get_string_value(config,"PUERTO_GAMECARD");
    tiempoReintentoConexion = (uint32_t)config_get_int_value(config,"TIEMPO_DE_REINTENTO_CONEXION");
    tiempoOperacion = (uint32_t)config_get_int_value(config,"TIEMPO_DE_REINTENTO_OPERACION");
    punto_montaje = config_get_string_value(config,"PUNTO_MONTAJE_TALLGRASS");

    
    crearDirectorio(punto_montaje);
    
    pathMetadata = agregarAPuntoMontaje("/Metadata/");
    pathFiles = agregarAPuntoMontaje("/Files/");
    pathBlocks = agregarAPuntoMontaje("/Blocks/");
    crearDirectorio(pathMetadata);
    crearDirectorio(pathFiles);
    crearDirectorio(pathBlocks);

    insertarMetadataEnPath(pathMetadata,"./assets/MetadataFS.txt");
    insertarMetadataEnPath(pathFiles, "./assets/MetadataDirectorioGeneral.txt");

    

    t_config* metadataFS = config_create(agregarADirectorioMetadataFS("Metadata.txt"));
    cantBloques = (uint32_t)config_get_int_value(metadataFS,"BLOCKS");
    sizeBloque = (uint32_t)config_get_int_value(metadataFS,"BLOCK_SIZE");
    magicNumber = config_get_string_value(metadataFS,"MAGIC_NUMBER");
    config_destroy(metadataFS);

    pathBitmap = agregarADirectorioMetadataFS("Bitmap.txt");
    printf("path bitmap: %s\n", pathBitmap);
    // if(existeBitmap(pathBitmap)){

    // }else{
    bitmapArr = crear_bitmap_en_disco(pathBitmap, cantBloques);
    // }
    semaforosPokemon = list_create();
    // sem_init(&semReconexion,0,1);

    // pthread_create(&hiloEscuchaBroker, NULL, escuchaPermanenteBroker,&idConexionPermanente);
    // pthread_join(hiloEscuchaBroker,NULL);
}
