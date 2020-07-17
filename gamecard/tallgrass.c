#include "tallgrass.h"

//devuelve el path, sin importar si ya esta creado o no
bool crearDirectorio(char *rutaDirectorio)
{
    bool existe;

    existe = mkdir(rutaDirectorio, 0777); //0777 da permiso a todos
    if (!existe)
    {
        log_info(logger, "Se creo el directorio %s", rutaDirectorio);
    }
    else
    {
        log_error(logger, "Ya existe el directorio %s en esta ruta", rutaDirectorio);
    }
    return existe;
}

bool existeDirectorio(char* rutaDirectorio){ //la ruta del direc tiene que terminar con un /
    struct stat s;
    int err = stat(rutaDirectorio, &s);
    if(-1 == err) {
        if(ENOENT == errno) {
            return false;
        } else {
            perror("stat"); //borrar esta parte despues
            exit(1);
        }
    } else {
        if(S_ISDIR(s.st_mode)) {
            return true;
        } else {
            return false;
        }
    }
}

char *agregarAPuntoMontaje(char *restoRuta)
{
    char *punto_montajeTemp = string_duplicate(punto_montaje);

    log_info(logger, punto_montajeTemp);
    string_append(&punto_montajeTemp, restoRuta);
    log_info(logger, punto_montajeTemp);
    return punto_montajeTemp;
}

char* agregarADirectorioFiles(char* restoRuta){
    char* files = string_duplicate(pathFiles);
    string_append(&files,restoRuta);
    return files;
}
char* agregarADirectorioBlocksInt(uint32_t nroBloque){
    char* blocks = string_duplicate(pathBlocks);
    string_append_with_format(&blocks, "%s.txt",string_itoa(nroBloque));
    return blocks;
}
char* agregarADirectorioBlocksChar(char* nroBloque){
    char* blocks = string_duplicate(pathBlocks);
    string_append_with_format(&blocks, "%s.txt",nroBloque);
    return blocks;
}

char* agregarADirectorioMetadataFS(char* restoRuta){
    char* metadata = string_duplicate(pathMetadata);
    string_append(&metadata, restoRuta);
    return metadata;
}

char* agregarADirectorioBitmap(char* restoRuta){
    char* bitmap = string_duplicate(pathBitmap);
    string_append(&bitmap, restoRuta);
    return bitmap;
}

//devuelve duplicado cosa de no pisar el path usado
char* agregarAPath(char* path, char* agregado){
    char* pathTemp = string_duplicate(path);
    string_append(&pathTemp, agregado);
    return pathTemp;
}

void escribirNewPokemon(char *textoNewPoke, char* pathMetadataPoke){
    char** keyAndValue = string_n_split(textoNewPoke, 2, "=");
    char* textoCompleto = leerArchivo(pathMetadataPoke);
    t_dictionary* dicPoke = crearDictionaryDeTexto(textoCompleto);

    if(dictionary_has_key(dicPoke,keyAndValue[0]) && strlen(textoCompleto)>1){
        uint32_t valorPrevioEnKey = obtenerIntDic(dicPoke,keyAndValue[0]);
        setValueDic(dicPoke,keyAndValue[0],string_itoa(valorPrevioEnKey + atoi(keyAndValue[1])));
        char* textoDic = dictionaryToString(dicPoke);
        modificarBloquesNewPoke(textoDic, pathMetadataPoke);
    }else{
        separarTextoEnBloques(textoNewPoke, pathMetadataPoke,false, "a+");
    }
    free(keyAndValue[0]);
    free(keyAndValue[1]);
    free(keyAndValue);
    dictionary_destroy_and_destroy_elements(dicPoke,free);
}

void escribirCatchPokemon(char *textoCatchPokemon, char* pathMetadataPoke ){
    printf("path metadata que se le pasa a escribir catch pokemon: %s\n", pathMetadataPoke);
    char** keyAndValue = string_n_split(textoCatchPokemon, 2, "=");
    char* textoCompleto = leerArchivo(pathMetadataPoke);
    uint32_t bytesBorrados;
    t_dictionary* dicPoke = crearDictionaryDeTexto(textoCompleto);
    if(dictionary_has_key(dicPoke,keyAndValue[0]) && strlen(textoCompleto)>1){
        uint32_t valorPrevioEnKey = obtenerIntDic(dicPoke,keyAndValue[0]);
        setValueDic(dicPoke,keyAndValue[0],string_itoa(valorPrevioEnKey - atoi(keyAndValue[1])));
        if(obtenerIntDic(dicPoke,keyAndValue[0]) == 0){
            bytesBorrados = strlen(textoCatchPokemon);
            removerKeyDic(dicPoke,keyAndValue[0]);
        }else{
            bytesBorrados = strlen(keyAndValue[1]) - strlen(string_itoa(valorPrevioEnKey));
        }
        char* textoDic = dictionaryToString(dicPoke);
        modificarBloquesCatchPoke(textoDic, pathMetadataPoke, bytesBorrados);
    }else{
        printf("no existe la posicion %s\n", keyAndValue[0]);
        //log error
    }
    free(keyAndValue[0]);
    free(keyAndValue[1]);
    free(keyAndValue);
    dictionary_destroy_and_destroy_elements(dicPoke,free);
}

t_localized_pokemon* escribirLocalizedPokemon(t_get_pokemon* getPoke, char* pathMetadataPoke){
    char* textoCompleto = leerArchivo(pathMetadataPoke);
    t_dictionary* dicPoke = crearDictionaryDeTexto(textoCompleto);
    t_list* listaPosiciones = dictionaryToListPosiciones(dicPoke);

    t_localized_pokemon* localizedPoke = crearLocalizedPokemon(0,getPoke->ID_mensaje_recibido, getPoke->nombre, list_size(listaPosiciones), listaPosiciones);
    return localizedPoke;
}



void separarTextoEnBloques(char *textoArchivo, char* pathMetadataPoke, bool modificarBloques, char* modoEscritura)
{
    char *textoArchivoTemp = string_duplicate(textoArchivo);
    uint32_t lengthTexto = strlen(textoArchivo);
    int sizeAEscribirPrimerBloque = 0;
    uint32_t bloqueAEscribir = 0;
    printf("size a escribir: %i\n", sizeAEscribirPrimerBloque);
    printf("bloque en el que escribir: %i\n", bloqueAEscribir);
    int i = 0;
    int j = 0;
    while (i != lengthTexto)
    {
        bloqueAEscribir = verQueBloqueEscribir(pathMetadataPoke, &sizeAEscribirPrimerBloque, modificarBloques, j);
        char *string;
        if(sizeAEscribirPrimerBloque < 0){
            printf("entra en primer if\n");
            string = string_substring(textoArchivoTemp,i, abs(sizeAEscribirPrimerBloque));
        }if(sizeAEscribirPrimerBloque > 0 && sizeAEscribirPrimerBloque < lengthTexto && i == 0){
            string = string_substring(textoArchivoTemp,i, sizeAEscribirPrimerBloque);
        }
        else{
            printf("entra en segundo if\n");
            string = string_substring(textoArchivoTemp, i, sizeBloque);
        }
        printf("Que estoy escribiendo ahora: %s\n", string);
        escribirBloque(string, pathMetadataPoke, bloqueAEscribir, modoEscritura);
        i += string_length(string);
        sleep(3);
        j++;
        
        
    }
}

uint32_t verQueBloqueEscribir(char* pathMetadataPoke,int* sizeAEscribirPrimerBloque, bool modificarBloques, uint32_t j){
    // uint32_t cantElementos;
    t_config* metadataPoke = config_create(pathMetadataPoke);
    char** arrBloques = config_get_array_value(metadataPoke,"BLOCKS");

     if(atoi(arrBloques[0]) == -1){
        return buscarBloqueEInsertarEnArchivo(metadataPoke);
    }else{
        // cantElementos = arraySize((void*)arrBloques);
        uint32_t sizeArchivo = config_get_int_value(metadataPoke,"SIZE");
        *sizeAEscribirPrimerBloque = calculoBytesSobrantes(arrBloques, sizeArchivo);
        printf("size a escribir: %i y cantidad de elementos: %i y el sizeArchivo: %i\n", *sizeAEscribirPrimerBloque, arraySize((void*)arrBloques), sizeArchivo);
        // sizeBloque*cantElementos - sizeArchivo; //calculo para ver cuanto espacio le queda al ultimo bloque del archivo, para decirle al separador que separe por eso la primera vez
        
        if(modificarBloques){
            return atoi(arrBloques[j]);
        }else{
            if(*sizeAEscribirPrimerBloque < 0){
                return buscarBloqueEInsertarEnArchivo(metadataPoke);
            }else{
                if(*sizeAEscribirPrimerBloque == 0){
                    return buscarBloqueEInsertarEnArchivo(metadataPoke);
                } else{
                    config_destroy(metadataPoke);
                    return atoi(arrBloques[arraySize((void*)arrBloques) -1]);
                }
            }
        }
    }
}

int calculoBytesSobrantes(char** arrBloques, uint32_t sizeArchivo){
    uint32_t cantElementos = arraySize((void*)arrBloques);
    return sizeBloque*cantElementos - sizeArchivo;
}

void modificarBloquesCatchPoke(char* textoCompleto, char* pathMetadataPoke, uint32_t bytesBorrados){
    t_config* metadataPoke = config_create(pathMetadataPoke);
    char** arrBloques = config_get_array_value(metadataPoke, "BLOCKS");
    uint32_t sizeArchivo = config_get_int_value(metadataPoke,"SIZE");
    uint32_t bytesSobrantes = calculoBytesSobrantes(arrBloques, sizeArchivo) + bytesBorrados;
    printf("BYTES SOBRANTES DE MODIFICAR CATCH POKE: %i\n", bytesSobrantes);

    if(bytesSobrantes >= sizeBloque){ //si los bytes sobrantes dan mayor a sizeBloque, significa que necesita menos bloques para guardar
        sacarUltimoBloqueDeArchivo(metadataPoke,arrBloques);
    }
    setSizeArchivo(metadataPoke, 0);
    separarTextoEnBloques(textoCompleto, pathMetadataPoke, true, "w");
    config_destroy(metadataPoke); 
}

//lo unico que hace esta funcion es agregar o no bloques necesarios al array de bloques del archivo y despues tirar el separar por bloques de nuevo, para que sobreescriba la info de todos los bloques que le pertenecen
void modificarBloquesNewPoke(char* textoCompleto, char* pathMetadataPoke){
    t_config* metadataPoke = config_create(pathMetadataPoke);
    char** arrBloques = config_get_array_value(metadataPoke, "BLOCKS");
    uint32_t sizeArchivo = config_get_int_value(metadataPoke,"SIZE");
    uint32_t bytesSobrantes = calculoBytesSobrantes(arrBloques, sizeArchivo);

    if(bytesSobrantes < 0){ //si los bytes sobrantes dan menor a 0, significa que necesita mas bloques para guardar
        for(uint32_t i = 0; i >= 0; i += sizeBloque){ //se hace el for de esta manera para que se inserten todos los bloques necesarios para que pueda guardar la info nueva satisfactoriamente
            buscarBloqueEInsertarEnArchivo(metadataPoke);
        } 
    } 
    setSizeArchivo(metadataPoke, 0);
    separarTextoEnBloques(textoCompleto, pathMetadataPoke, true, "w");
    config_destroy(metadataPoke);
}



uint32_t buscarBloqueEInsertarEnArchivo(t_config* metadataPoke){
    uint32_t bloqueLibre = buscarBloqueLibre();
    insertarBloqueEnArchivo(metadataPoke, config_get_array_value(metadataPoke,"BLOCKS"),bloqueLibre);
    config_destroy(metadataPoke);
    return bloqueLibre;
}


void sacarUltimoBloqueDeArchivo(t_config* metadataPoke, char** arrBloques){
    t_list* listaBloques = arrayToList((void*)arrBloques);
    remove(agregarADirectorioBlocksChar(list_get(listaBloques,list_size(listaBloques)-1)));
    //mutex
    bitarray_clean_bit(bitmapArr,list_size(listaBloques)-1);
    //mutex
    list_remove_and_destroy_element(listaBloques,list_size(listaBloques)-1, free);
    config_set_value(metadataPoke,"BLOCKS", listToString(listaBloques));
    config_save(metadataPoke);
}

void insertarBloqueEnArchivo(t_config* metadataPoke, char** arrBloques, uint32_t bloque){
    t_list* listaBloques = arrayToList((void*)arrBloques);
    if(atoi(list_get(listaBloques, 0)) == -1){
        list_remove(listaBloques,0);
        list_add(listaBloques,string_itoa(bloque));
        char* strDUp = listToString(listaBloques);
        config_set_value(metadataPoke,"BLOCKS", strDUp);
        config_save(metadataPoke);
    }else{
        list_add(listaBloques, string_itoa(bloque));
        config_set_value(metadataPoke,"BLOCKS", listToString(listaBloques));
        config_save(metadataPoke);
    }
}

void setSizeArchivo(t_config* metadataPoke, uint32_t sizeNuevo){
    config_set_value(metadataPoke, "SIZE", string_itoa(sizeNuevo));
    config_save(metadataPoke);
}

void modificarOpenArchivo(char* metadataPoke, char* valorOpen){
    t_config* configMetaPoke = config_create(metadataPoke);
    config_set_value(configMetaPoke, "OPEN", valorOpen);
    config_save(configMetaPoke);
    config_destroy(configMetaPoke);
}

void escribirBloque(char *texto, char* pathMetadataPoke, uint32_t bloqueAEscribir, char* modoEscritura)
{
    FILE *fp;
    t_config* metadataPoke = config_create(pathMetadataPoke);
    uint32_t sizeArchivo = (uint32_t)config_get_int_value(metadataPoke,"SIZE");
    char *path = string_duplicate(pathBlocks);

    string_append_with_format(&path, "%s.txt",string_itoa(bloqueAEscribir));

    fp = fopen(path, modoEscritura);

    if (fp == NULL)
    {
        // printf("el path %s no se pudo abrir\n", path);
    }
    else
    {
        // printf("El archivo %s se encuentra abierto\n", path);
    }
    if (strlen(texto) > 0)
    {
        fputs(texto, fp);
        // if(string_contains(modoEscritura, "a")){
        setSizeArchivo(metadataPoke, sizeArchivo + strlen(texto));
        // }
    }
    fclose(fp);
    config_destroy(metadataPoke);
    // printf("Se logro escribir en %s el texto %s y se cerro el archivo\n", path, texto);
}

char* leerArchivo(char *pathMetadataPoke)
{
    FILE *fp;
    t_config* metadataPoke = config_create(pathMetadataPoke);
    char **arrBloques = config_get_array_value(metadataPoke,"BLOCKS");
    // int sizeArchivo = config_get_int_value(metadataPoke,"SIZE");
    char *path = string_duplicate(pathBlocks);
    char* textoCompleto = string_new();
    char textoTemp[sizeBloque+1];

    if(atoi(arrBloques[0]) != -1){
        printf("arr size: %i\n", arraySize((void*)arrBloques));
        for (int i = 0; i < arraySize((void*)arrBloques); i++)
        {
            printf("elemento arr: %s\n", arrBloques[i]);
            path = agregarADirectorioBlocksChar(arrBloques[i]);
            printf("path: %s\n", path);

            fp = fopen(path, "r");

            if (fp == NULL)
            {
                printf("GfgTest.c file failed to open.\n");
                return "";
            }
            else
            {    
                while (fgets(textoTemp, sizeBloque+1, fp) != NULL)
                {
                    string_append(&textoCompleto, textoTemp);
                }
                fflush(fp);
                fclose(fp);
            }
        }
    }else{
        return "";
    }
    config_destroy(metadataPoke);
    printf("texto completo desde leer archivo: %s\n", textoCompleto);
    return textoCompleto;
}

void escribirMetadata(char* path, char* aEscribir)
{
    FILE *filePointer;
    filePointer = fopen(path, "w");

    if (filePointer == NULL)
    {
        // printf("Pokemon.bin file failed to open.");
    }
    else
    {
        // printf("The file is now opened.\n");

        if (strlen(aEscribir) > 0)
        {
            fprintf(filePointer,"%s",aEscribir);
        }
        fclose(filePointer);
        // printf("Data successfully written in file GfgTest.c\n");
        // printf("The file is now closed.");
    }
}

char* leerMetadata(char* path)
{
    FILE *filePointer;

    filePointer = fopen(path, "r");
    struct stat st;
    stat(path, &st);

    char* dataToBeRead = malloc(st.st_size); //si algo no funca es esto
    char* textoCompleto = string_new();

    if (filePointer == NULL)
    {
        // printf("GfgTest.c file failed to open.");
    }
    else
    {
        // printf("The file is now opened.\n");
        while (fgets(dataToBeRead, st.st_size, filePointer) != NULL)
        {
            string_append(&textoCompleto, dataToBeRead);
        }
        fclose(filePointer);
        free(dataToBeRead); 
    }
        return textoCompleto;
}

bool existeBitmap(char* rutaArchivo){
    struct stat st;   
    return (stat (rutaArchivo, &st) == 0);
}

t_bitarray *crear_bitmap_en_disco(char *archivo, size_t size)
{
    int fd = open(archivo, O_CREAT | O_RDWR, 0664);
    if (fd == -1)
    {
        perror("open file");
        exit(1);
    }
    ftruncate(fd, size);

    bmap = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (bmap == MAP_FAILED)
    {
        perror("mmap");
        close(fd);
        exit(1);
    }

    t_bitarray *bitmap = bitarray_create_with_mode((char *)bmap, size / 8, LSB_FIRST);
    size_t tope = bitarray_get_max_bit(bitmap);

    for (int i = 0; i < tope; i++)
    {
        bitarray_clean_bit(bitmap, i);
    }

    // msync(NULL, size, LSB_FIRST);
    close(fd);
    // munmap(bmap, size); para finalizar gamecard
    return bitmap;
}

uint32_t buscarBloqueLibre()
{
    // t_bitarray *bitmap = bitarray_create_with_mode((char *)bmap, cantBloques / 8, LSB_FIRST);
    uint32_t i = 0;
    while (bitarray_test_bit(bitmapArr, i) != 0)
    {
        i++;
    }
    bitarray_set_bit(bitmapArr,i);
    printf("el bit %i esta %i\n",i, bitarray_test_bit(bitmapArr,i));
    return i;
}

void insertarMetadataEnPath(char* pathDirectorio, char* archivoMetadata){
    char* aEscribir = leerMetadata(archivoMetadata);
    char* archivo = string_new();
    string_append(&archivo,pathDirectorio);
    string_append(&archivo, "/Metadata.txt"); 
   
    escribirMetadata(archivo, aEscribir);
}

char* newPokemonATexto(t_new_pokemon* newPokemon){
    char* textoCompleto = string_new();
    string_append(&textoCompleto, string_itoa(newPokemon->posicionCantidad->posicion_x));
    string_append(&textoCompleto,"-");
    string_append(&textoCompleto, string_itoa(newPokemon->posicionCantidad->posicion_y));
    string_append(&textoCompleto, "=");
    string_append(&textoCompleto, string_itoa(newPokemon->posicionCantidad->cantidad));
    string_append(&textoCompleto,"\n");
    return textoCompleto;
}
char* catchPokemonATexto(t_catch_pokemon* catchPokemon){
    char* textoCompleto = string_new();
    string_append(&textoCompleto, string_itoa(catchPokemon->posicion->posicion_x));
    string_append(&textoCompleto,"-");
    string_append(&textoCompleto, string_itoa(catchPokemon->posicion->posicion_y));
    string_append(&textoCompleto, "=");
    string_append(&textoCompleto, "1"); //asi hago la resta mas facil despues
    string_append(&textoCompleto,"\n");
    return textoCompleto;
}



t_localized_pokemon* atenderGetPokemon(t_get_pokemon* getPokemon){
    t_localized_pokemon* localizedPokemon;
    char* pathFilesPokemon = string_duplicate(pathFiles);
    string_append(&pathFilesPokemon,getPokemon->nombre);
    char* pathMetadataPoke = agregarAPath(pathFilesPokemon,"/Metadata.txt");
    if(existeDirectorio(pathFilesPokemon)){
        localizedPokemon = escribirLocalizedPokemon(getPokemon, pathMetadataPoke);
    }else{
        //sin cantidad de posiciones, y sin posiciones
        localizedPokemon = crearLocalizedPokemon(0, getPokemon->ID_mensaje_recibido, getPokemon->nombre, 0,list_create());
    }
    return localizedPokemon;
}

void atenderCatchPokemon(t_catch_pokemon* catchPokemon){
    char* pathFilesPokemon = string_duplicate(pathFiles);
    string_append(&pathFilesPokemon,catchPokemon->nombre);
    char* pathMetadataPoke = agregarAPath(pathFilesPokemon,"/Metadata.txt");
    if(existeDirectorio(pathFilesPokemon)){
        //verificar si se puede abrir el archivo
        escribirCatchPokemon(catchPokemonATexto(catchPokemon), pathMetadataPoke);
    }else{
        printf("No existe el directorio %s\n", pathFilesPokemon);
        //informar error en log
    }
    
}

//se deserializa en el processrequest de gamecard.c
void* atenderNewPokemon(void* newPokemonParam){
    t_new_pokemon* newPokemon = (t_new_pokemon*)newPokemonParam;
    char* pathFilesPokemon = string_duplicate(pathFiles);
    string_append(&pathFilesPokemon,newPokemon->nombre);
    char* pathMetadataPoke = agregarAPath(pathFilesPokemon,"/Metadata.txt");
    t_config* metadataPoke = config_create(pathMetadataPoke);
    uint32_t indexSemaforo = 0;

    bool yaExistia = crearDirectorio(pathFilesPokemon);
    if(!yaExistia){
        insertarMetadataEnPath(pathFilesPokemon,"./assets/MetadataArchivoGeneral.txt");
        t_semaforo_pokemon* semPoke = crearSemaforoPokemon(newPokemon->nombre);
        sem_wait(&(semPoke->semPoke));
        modificarOpenArchivo(pathMetadataPoke,"Y");
        indexSemaforo = list_add(semaforosPokemon,semPoke);
        log_info(logger,"el index de la lista de semaforos en el if: %i",indexSemaforo);
        // escribirNewPokemon(newPokemonATexto(newPokemon), pathMetadataPoke);
    }else{
        if(estaOpen(metadataPoke)){
            log_info(logger,"entra en IF de si esta open");
            pthread_t hiloRecuperacion;
            pthread_create(&hiloRecuperacion, NULL, reintentarOperacion,metadataPoke);
            pthread_detach(hiloRecuperacion);
        }
        indexSemaforo = encontrarSemaforoDelPoke(newPokemon->nombre,semaforosPokemon);
        log_info(logger,"el index de la lista de semaforos en el else: %i",indexSemaforo);
        sem_wait(&(((t_semaforo_pokemon*)list_get(semaforosPokemon, indexSemaforo))->semPoke));

        modificarOpenArchivo(pathMetadataPoke,"Y");
    }
    escribirNewPokemon(newPokemonATexto(newPokemon), pathMetadataPoke);
    sem_post(&(((t_semaforo_pokemon*)list_get(semaforosPokemon, indexSemaforo))->semPoke)); 
    sleep(tiempoOperacion);
    modificarOpenArchivo(pathMetadataPoke,"N");
    printf("termino la operacioon\n");
    sleep(tiempoOperacion);
    return NULL;
}

bool estaOpen(t_config* metadataPoke){
    char* valorOpen = config_get_string_value(metadataPoke,"OPEN");
    if(string_equals_ignore_case(valorOpen, "Y")){
        printf("ESTA OPEN\n");
        log_info(logger,"ESTA OPEN con valor: %s",config_get_string_value(metadataPoke,"OPEN"));
        return true;
    }else{
        printf("NO ESTA OPEN\n");
        log_info(logger,"NO ESTA OPEN con valor %s", config_get_string_value(metadataPoke,"OPEN"));
        return false;
    }
}

void* reintentarOperacion(void* metadataPoke){
    t_config* configMetadataPoke = (t_config*)metadataPoke;
    bool archivoCerrado = false;
    uint32_t i = 0;
    while(archivoCerrado){
        sleep(tiempoReintentoConexion);
        archivoCerrado = !estaOpen(configMetadataPoke);
        i++;
        printf("reintento numero %i\n", i);
        log_info(logger, "reintento numeo %i", i);
    }
    return false;
}