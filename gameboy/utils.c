
#include "utils.h"

//TODO
/*
 * Recibe un paquete a serializar, y un puntero a un int en el que dejar
 * el tamaño del stream de bytes serializados que devuelve
 */
// void* serializar_paquete(t_paquete* paquete, int bytes)
// {
// 	void * magic = malloc(bytes);
// 	int desplazamiento = 0;

// 	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
// 	desplazamiento+= sizeof(int);
// 	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
// 	desplazamiento+= sizeof(int);
// 	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
// 	desplazamiento+= paquete->buffer->size;

// 	return magic;
// }

// int crear_conexion(char *ip, char* puerto)
// {
// 	struct addrinfo hints;
// 	struct addrinfo *server_info;

// 	memset(&hints, 0, sizeof(hints));
// 	hints.ai_family = AF_UNSPEC;
// 	hints.ai_socktype = SOCK_STREAM;
// 	hints.ai_flags = AI_PASSIVE;

// 	getaddrinfo(ip, puerto, &hints, &server_info);

// 	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

// 	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1)
// 		printf("error");

// 	freeaddrinfo(server_info);

// 	return socket_cliente;
// }

// //TODO
// void enviar_mensaje(char* mensaje, int socket_cliente)
// {
// 	t_paquete* paquete = malloc(sizeof(t_paquete));

// 	paquete->codigo_operacion = MENSAJE;
// 	paquete->buffer = malloc(sizeof(t_buffer));
// 	paquete->buffer->size = strlen(mensaje)+1;
// 	paquete->buffer->stream = malloc(paquete->buffer->size);
// 	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

// 	int sizePaquete = paquete->buffer->size + 2 * sizeof(int);

// 	void* a_enviar = serializar_paquete(paquete,sizePaquete);

// 	send(socket_cliente,a_enviar,sizePaquete,0);

// 	free(a_enviar);
// 	free(paquete->buffer->stream);
// 	free(paquete->buffer);
// 	free(paquete);

// }

// //TODO
// char* recibir_mensaje(int socket_cliente)
// {
// 	int cod_op, size;
// 	char* mensaje;
// 	recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL);

// 	if(cod_op == MENSAJE) {
// 		recv(socket_cliente, &size, sizeof(int), MSG_WAITALL);
// 		mensaje = malloc(size);
// 		recv(socket_cliente, mensaje, size, MSG_WAITALL);

// 		return mensaje;
// 	} else {
// 		return "No recibi nada";
// 	}
// }

// void liberar_conexion(int socket_cliente)
// {
// 	close(socket_cliente);
// }


uint32_t sizeOfArgumentos(int lengthArray, char* argumentos[], char* arrayTemp[]){
	uint32_t size_buffer = 0;
	uint32_t j = 0;

	for(uint32_t i = 3; i < lengthArray; i++ ){
		size_buffer += strlen(argumentos[i]) +1;
		arrayTemp[j] = argumentos[i];
		j++;
	}
	return size_buffer;
}

void procesarComando(int argc, char* argv[]) {
	
	char* proceso = argv[1];
	char* tipo_mensaje = argv[2];
	char* ip;
	char* puerto;
	char* arrayTemporal[argc-3];
	char* brokerCommands[] = {"NEW_POKEMON","APPEARED_POKEMON","CATCH_POKEMON","CAUGHT_POKEMON","GET_POKEMON"};
	char* teamCommands[] = {"APPEARED_POKEMON"};
	char* gamecardCommands[] = {"NEW_POKEMON","CATCH_POKEMON","GET_POKEMON"};
	uint32_t isValid = 0;

	if(strcmp(proceso,"BROKER") == 0 || strcmp(proceso,"SUSCRIPTOR") == 0) {
		ip = config_get_string_value(config,"IP_BROKER");
		puerto = config_get_string_value(config,"PUERTO_BROKER");
		if(perteneceAlArray(tipo_mensaje,brokerCommands,5) == 1) {
			isValid = 1;
		}
	}
	else if(strcmp(proceso,"TEAM") == 0) {
		ip = config_get_string_value(config,"IP_TEAM");
		puerto = config_get_string_value(config,"PUERTO_TEAM");
		if(perteneceAlArray(tipo_mensaje,teamCommands,1) == 1) {
			isValid = 1;
		}
	}
	else if(strcmp(proceso,"GAMECARD") == 0) {
		ip = config_get_string_value(config,"IP_GAMECARD");
		puerto = config_get_string_value(config,"PUERTO_GAMECARD");
		if(perteneceAlArray(tipo_mensaje,gamecardCommands,3) == 1) {
			isValid = 1;
		}
	}

	log_info(logger,ip);
	log_info(logger,puerto);

	if(isValid == 1) {
		t_paquete* paquete = malloc(sizeof(t_paquete));
		paquete->buffer = malloc(sizeof(t_buffer));
		if(strcmp(tipo_mensaje,"NEW_POKEMON") == 0) {
			log_info(logger, "entro a new_pokemon");
			
			uint32_t sizeOfArgs = sizeOfArgumentos(argc, argv, arrayTemporal);
			//REVISAR QUE HACER EXACTAMENTE CON ESTA FUNCION, ME DA PAJA AHORA

			t_new_pokemon* newPokemon = malloc(sizeof(t_new_pokemon));
			t_posicion_cantidad* posCant = malloc(sizeof(t_posicion_cantidad));

			posCant->posicion_x=(uint32_t)atoi(argv[4]); //se usa el atoi para pasar de char* a int, despues casteo a uint32_t ya que todos nuestros datos son de ese tipo
			posCant->posicion_y=(uint32_t)atoi(argv[5]);
			posCant->cantidad=(uint32_t)atoi(argv[6]);


			newPokemon->ID_mensaje_recibido = 1234;
			newPokemon->nombre=argv[3];
			newPokemon->sizeNombre = strlen(newPokemon->nombre) +1;
			newPokemon->posicionCantidad = posCant;
			
			

			paquete = serializar_newPokemon(newPokemon);
			
			t_new_pokemon* pikachu = deserializar_newPokemon(paquete->buffer);
			printf("nombre del poke: %s", pikachu->nombre);
			

			free(paquete->buffer->stream);
			free(paquete->buffer);
			free(paquete);
		}
		else if(strcmp(tipo_mensaje,"APPEARED_POKEMON") == 0) {
			log_info(logger, "entro a appeared_pokemon");
		}
		else if(strcmp(tipo_mensaje,"CATCH_POKEMON") == 0) {
			log_info(logger, "entro a catch_pokemon");
		}
		else if(strcmp(tipo_mensaje,"CAUGHT_POKEMON") == 0) {
			log_info(logger, "entro a caught_pokemon");
		}
		else if(strcmp(tipo_mensaje,"GET_POKEMON") == 0) {
			log_info(logger, "entro a get_pokemon");
		}
		else if(strcmp(tipo_mensaje,"LOCALIZED_POKEMON") == 0) {//ESTE AL FINAL NO LO HACE GAMEBOY, PARECE SER LA UNICA MQ QUE NO -> PERO LO DEJO IGUAL ASI PRUEBO LA SERIALIZACION Y LA DESERIALIZACION (DESPUES SACAR THO)
			log_info(logger, "entro a localized_pokemon");
			uint32_t sizeOfArgs = sizeOfArgumentos(argc, argv, arrayTemporal);
			paquete->buffer->size = sizeOfArgs;
			paquete->buffer->stream = malloc(paquete->buffer->size);

			t_localized_pokemon* localizedPokemon = malloc(sizeof(t_localized_pokemon));
			
			t_posicion_cantidad* posCant1 = crearPosicionCantidad(5,10,6);
			t_posicion_cantidad* posCant2 = crearPosicionCantidad(8,11,6);
			
			t_list* listPosCant = list_create();
			list_add(listPosCant, posCant1);
			list_add(listPosCant, posCant2);

			localizedPokemon->ID_mensaje_recibido_originalmente = 12342;
			localizedPokemon->nombre = "lucario";
			localizedPokemon->sizePokemon= strlen(localizedPokemon->nombre)+1;
			localizedPokemon->sizePosicion_cantidad = list_size(listPosCant)*sizeof(t_posicion_cantidad);
			localizedPokemon->posicion_cantidad = listPosCant;

			paquete=serializar_localizedPokemon(localizedPokemon); //COMENTADO PORQUE OBVIO QUE LA SERIALIZACION DE LA LISTA ROMPE TODO

			t_localized_pokemon* lucario = deserializar_localizedPokemon(paquete->buffer);
			printf("nombre del poke: %s \n", lucario->nombre);

			t_posicion_cantidad* posicion = list_get(lucario->posicion_cantidad,1);
			printf("la primera posicion es %i:%i\n",posicion->posicion_x,posicion->posicion_y);
		}
		
	} else {
		printf("el comando no es valido para el modulo %s\n",proceso);
	}
}

uint32_t perteneceAlArray(char* val, char* arr[], uint32_t size)
{
    for(uint32_t i = 0; i < size; i++)
    {
		if(strcmp(arr[i],val) == 0)
            return 1;
    }
    return 0;
}