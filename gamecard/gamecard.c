#include "gamecard.h"


int main(void)
{
    config = config_create("./assets/gamecard.config");

	logger = log_create("./assets/gamecard.log","gamecard",true,LOG_LEVEL_INFO);

    log_info(logger, "a ver");

    inicializarGamecard();
  
      if(pthread_create(&threadSUSCRIBE_CATCH,NULL,(void*)suscribe,(void*)CATCH_POKEMON) != 0)
        printf("Error");

    if(pthread_create(&threadSUSCRIBE_NEW,NULL,(void*)suscribe,(void*)NEW_POKEMON) != 0)
        printf("Error");

    if(pthread_create(&threadSUSCRIBE_GET,NULL,(void*)suscribe,(void*)GET_POKEMON) != 0)
        printf("Error");

    if (pthread_create(&threadSERVER,NULL,(void*)iniciar_servidor,NULL) != 0)
        printf("Error");
    


    // sem_init(&semReconexion,0,1);

    // pthread_create(&hiloEscuchaBroker, NULL, escuchaPermanenteBroker,&idConexionPermanente);
    // pthread_join(hiloEscuchaBroker,NULL);

    terminar_programa(logger,config);
    return 0;
}

void terminar_programa(t_log* logger, t_config* config){
    pthread_join(threadSUSCRIBE_CATCH, NULL);
    pthread_join(threadSUSCRIBE_NEW, NULL);
    pthread_join(threadSUSCRIBE_GET, NULL);
    pthread_join(threadSERVER, NULL);

    log_destroy(logger);
    config_destroy(config);
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

