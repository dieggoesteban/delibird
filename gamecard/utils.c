
#include "utils.h"

void cortarArgumentos(int lengthArray, char *argumentos[], char *arrayTemp[])
{
	uint32_t j = 0;

	for (uint32_t i = 3; i < lengthArray; i++)
	{
		arrayTemp[j] = argumentos[i];
		j++;
	}
}

uint32_t procesarComando(char** ip, char** puerto, char *proceso, char *tipo_mensaje) {
	uint32_t isValid = 0;
	char *brokerCommands[] = {"NEW_POKEMON", "APPEARED_POKEMON", "CATCH_POKEMON", "CAUGHT_POKEMON", "GET_POKEMON"};
	char *teamCommands[] = {"APPEARED_POKEMON"};
	char *gamecardCommands[] = {"NEW_POKEMON", "CATCH_POKEMON", "GET_POKEMON"};
	char *ipAux, *puertoAux;

	if (strcmp(proceso, "BROKER") == 0 || strcmp(proceso, "SUSCRIPTOR") == 0)
	{
		ipAux = config_get_string_value(config, "IP_BROKER");
		puertoAux = config_get_string_value(config, "PUERTO_BROKER");
		log_info(logger,"Se va a conectar en BROKER");
		if (perteneceAlArray(tipo_mensaje, brokerCommands, 5) == 1)
		{
			isValid = 1;
		}
	}
	else if (strcmp(proceso, "TEAM") == 0)
	{
		ipAux = config_get_string_value(config, "IP_TEAM");
		puertoAux = config_get_string_value(config, "PUERTO_TEAM");
		log_info(logger,"Se va a conectar en TEAM");
		if (perteneceAlArray(tipo_mensaje, teamCommands, 1) == 1)
		{
			isValid = 1;
		}
	}
	else if (strcmp(proceso, "GAMECARD") == 0)
	{
		ipAux = config_get_string_value(config, "IP_GAMECARD");
		puertoAux = config_get_string_value(config, "PUERTO_GAMECARD");
		log_info(logger,"Se va a conectar en GAMECARD");
		if (perteneceAlArray(tipo_mensaje, gamecardCommands, 3) == 1)
		{
			isValid = 1;
		}
	}

	*ip = malloc(strlen(ipAux));
	*puerto = malloc(strlen(puertoAux));
	strcpy(*ip, ipAux);
	strcpy(*puerto, puertoAux);

	return isValid;
}

uint32_t perteneceAlArray(char *val, char *arr[], uint32_t size)
{
	for (uint32_t i = 0; i < size; i++)
	{
		if (strcmp(arr[i], val) == 0)
			return 1;
	}
	return 0;
}

//robadisimo
t_dictionary* crearDictionaryDeTexto(char* textoArchivo){
	t_dictionary* dicArchivo = dictionary_create();
	char** lines = string_split(textoArchivo, "\n");
	
	void add_cofiguration(char *line){
		if (!string_starts_with(line, "#")) {
			char** keyAndValue = string_n_split(line, 2, "=");
			dictionary_put(dicArchivo, keyAndValue[0], keyAndValue[1]);
			free(keyAndValue[0]);
			free(keyAndValue);
		}
	}
	string_iterate_lines(lines, add_cofiguration);
	string_iterate_lines(lines, (void*) free);

	free(lines);
	return dicArchivo;
}




char *obtenerStringDic(t_dictionary* dic, char *key) {
	return (char*)dictionary_get(dic, key);
}

int obtenerIntDic(t_dictionary *dic, char *key) {
	char *value = obtenerStringDic(dic, key);
	return atoi(value);
}

long obtenerLongDic(t_dictionary *dic, char *key) {
	char *value = obtenerStringDic(dic, key);
	return atol(value);
}

double obtenerDoubleDic(t_dictionary *dic, char *key) {
	char *value = obtenerStringDic(dic, key);
	return atof(value);
}

char** obtenerArrayCharDic(t_dictionary* dic, char* key) {
	char* value_in_dictionary = obtenerStringDic(dic, key);
	return string_get_string_as_array(value_in_dictionary);
}

void setValueDic(t_dictionary *dic, char *key, char *value) {
	char* duplicate_value = string_duplicate(value);

	dictionary_put(dic, key, (void*)duplicate_value);
}

void removerKeyDic(t_dictionary *dic, char *key) {

	if(dictionary_has_key(dic, key)) {
		dictionary_remove_and_destroy(dic, key, free);
	}
}


uint32_t encontrarSemaforoDelPoke(char* nombrePoke, t_list* listaSem){
	for(uint32_t i = 0; i < list_size(listaSem); i++){
		if( string_equals_ignore_case(((t_semaforo_pokemon*)list_get(listaSem,i))->nombrePoke, nombrePoke)){
			return i;
		}
	}
	return 0;
}

char* dictionaryToString(t_dictionary* dictionary){
	char* textoCompleto = string_new();
	printf("llega a entrar a la funcion");

	void keyAndValueToString(char* key, void* value){
		string_append_with_format(&textoCompleto, "%s=", key);
		string_append_with_format(&textoCompleto,"%s\n",(char*)value);
		printf("esto transformo la funcion: %s\n", textoCompleto);
	}
	dictionary_iterator(dictionary,keyAndValueToString);
	return textoCompleto;
}

t_list* dictionaryToListPosiciones(t_dictionary* dictionary){
	t_list* listaPosiciones = list_create();

	void separarKeyEnPosicion(char* key, void* value){
		char** keySeparada = string_split(key,"-");
		t_posicion* posicion = crearPosicion(atoi(keySeparada[0]), atoi(keySeparada[1]));
		list_add(listaPosiciones, posicion);
	}
	dictionary_iterator(dictionary,separarKeyEnPosicion);
	return listaPosiciones;
}

char* getMQName(uint32_t mq) {
	char* message;
	switch(mq) {
		case 1:
			message = "NEW_POKEMON";
			break;
		case 3:
			message = "CATCH_POKEMON";
			break;
		case 5:
			message = "GET_POKEMON";
			break;
	}
	return message;
}

t_suscribe* getSuscribe(uint32_t mq) {
	printf("Intentando SUSCRIBE en %s...\n", getMQName(mq));
	t_suscribe* suscribe = malloc(sizeof(t_suscribe));
    uint32_t conexion = escuchaBroker();
	printf("la Conexion es: %i y el MQ es %i\n", conexion,mq);

	suscribe->conexion = conexion;
	suscribe->messageQueue = mq;

	return suscribe;
}

uint32_t getIndexSemaforo(char* nombrePoke,t_list* lista) {
	for(uint32_t i = 0; i < list_size(lista); i++) {
		t_semaforo_pokemon* semPoke = (t_semaforo_pokemon*)list_get(lista, i);
		if(string_equals_ignore_case(semPoke->nombrePoke, nombrePoke)) {
			return i;
		}
	}
	return ERROR;
}