#include "gamecard.h"

void handler() {
    sem_post(&waitForFinish);
}

int main(int argc, char *argv[])
{
    config = config_create("./assets/gamecard.config");

	logger = log_create("./assets/gamecard.log","gamecard",true,LOG_LEVEL_INFO);

    if(argv[1]) {
        idModule = atoi(argv[1]);
    }
    else {
        idModule = (uint32_t)config_get_int_value(config,"ID_MODULE");
    }


    sem_init(&waitForFinish, 0, 0);
    sem_init(&mutexReconnect, 0, 0);

    signal(SIGINT, handler);

    inicializarGamecard();
    establecerConexionBroker();
  
    if (pthread_create(&finalizarPrograma,NULL,(void*)terminar_programa,NULL) != 0)
        printf("Error FINALIZAR\n");

    if (pthread_create(&threadDETECT_DISCON,NULL,(void*)detectarDesconexion,NULL) != 0)
        printf("Error DETECTOR\n");

    if (pthread_create(&threadRECONNECT,NULL,(void*)reconectarBroker,NULL) != 0)
        printf("Error RECONEXION\n");

    if (pthread_create(&threadSERVER,NULL,(void*)iniciar_servidor,NULL) != 0)
        printf("Error");
    

    pthread_detach(finalizarPrograma);
    pthread_join(threadRECONNECT, NULL);    
    pthread_join(threadDETECT_DISCON, NULL);
    pthread_join(threadSERVER, NULL);

    terminar_programa();

    return 0;
}

void terminar_programa(){
    sem_wait(&waitForFinish);
    printf("\nCERRANDO EL PROGRAMA*********************************\n");
    list_destroy_and_destroy_elements(semaforosPokemon, free);

    liberar_conexion(suscribeNew->conexion);
    liberar_conexion(suscribeCatch->conexion);
    liberar_conexion(suscribeGet->conexion);

    free(suscribeNew);
    free(suscribeCatch);
    free(suscribeGet);

    sem_destroy(&mutexReconnect);
    
    log_info(logger,"Liberamos todo");
    log_destroy(logger);
    config_destroy(config);

    exit(0);
}


void inicializarGamecard(){
    IP = config_get_string_value(config,"IP_GAMECARD");
    PUERTO = config_get_string_value(config,"PUERTO_GAMECARD");
    IP_BROKER = config_get_string_value(config, "IP_BROKER");
    PUERTO_BROKER = config_get_string_value(config,"PUERTO_BROKER");
    tiempoReintentoConexion = (uint32_t)config_get_int_value(config,"TIEMPO_DE_REINTENTO_CONEXION");
    tiempoOperacion = (uint32_t)config_get_int_value(config,"TIEMPO_DE_REINTENTO_OPERACION");
    tiempoReintentoOperacion = (uint32_t)config_get_int_value(config,"TIEMPO_RETARDO_OPERACION");
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
    if(existeArchivo(pathBitmap)){
        bitmapArr = copiar_bitmap_en_disco(pathBitmap, cantBloques);
    }else{
        bitmapArr = crear_bitmap_en_disco(pathBitmap, cantBloques);
    }
    semaforosPokemon = list_create();
    pthread_mutex_init(&mutexBitmap, NULL);

}