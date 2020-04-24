
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
