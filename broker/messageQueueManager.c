#include "messageQueueManager.h"

t_message_queue* crearMessageQueue(uint32_t mq_cod)
{
    t_message_queue* newMessageQueue = (t_message_queue*)malloc(sizeof(t_message_queue));

	switch (mq_cod)
	{
		case NEW_POKEMON:
			strcpy(newMessageQueue->name, "NEW_POKEMON");
			break;
		case APPEARED_POKEMON:
			strcpy(newMessageQueue->name, "APPEARED_POKEMON");
			break;
		case GET_POKEMON:
			strcpy(newMessageQueue->name, "GET_POKEMON");
			break;
		case CAUGHT_POKEMON:
			strcpy(newMessageQueue->name, "CAUGHT_POKEMON");
			break;
		case LOCALIZED_POKEMON:
			strcpy(newMessageQueue->name, "LOCALIZED_POKEMON");
			break;
		case CATCH_POKEMON:
			strcpy(newMessageQueue->name, "CATCH_POKEMON");
			break;
	}

    newMessageQueue->mq_cod = mq_cod;
    newMessageQueue->mensajes = list_create();
    newMessageQueue->subscribers = list_create();

	if(pthread_mutex_init(&newMessageQueue->s_mensajes, NULL) != 0)
		log_error(broker_custom_logger, "Error mutex init (s_mensajes) in crearMessageQueue");
	if(pthread_mutex_init(&newMessageQueue->s_subscribers, NULL) != 0)
		log_error(broker_custom_logger, "Error mutex init (s_subscribers) in crearMessageQueue");
	sem_init(&newMessageQueue->s_hayMensajes, 0, 0);

    return newMessageQueue;
}

t_message* crearMessage() 
{
    //TODO: Asignar ID a mensaje
    t_message* mensajeStruct = malloc(sizeof(t_message));
    mensajeStruct->suscriptoresConfirmados = list_create();
	mensajeStruct->suscriptoresEnviados = list_create();

	if(pthread_mutex_init(&mensajeStruct->s_suscriptoresEnviados, NULL) != 0)
		log_error(broker_custom_logger, "Error mutex init (s_suscriptoresEnviados) in crearMessage");

	if(pthread_mutex_init(&mensajeStruct->s_suscriptoresConfirmados, NULL) != 0)
		log_error(broker_custom_logger, "Error mutex init (s_suscriptoresConfirmados) in crearMessage");	
	
	sem_init(&mensajeStruct->s_puedeEliminarse, 0, 0);

    return mensajeStruct;
}

t_message_queue* getMessageQueueById(uint32_t id) 
{
	switch (id)
	{
		case NEW_POKEMON:
			return newPokemonMessageQueue;
			break;
		case APPEARED_POKEMON:
			return appearedPokemonMessageQueue;
			break;
		case GET_POKEMON:
			return getPokemonMessageQueue;
			break;
		case CAUGHT_POKEMON:
			return caughtPokemonMessageQueue;
			break;
		case LOCALIZED_POKEMON:
			return localizedPokemonMessageQueue;
			break;
		case CATCH_POKEMON:
			return catchPokemonMessageQueue;
			break;
		default:
		return NULL;
	}
}

t_list* getSuscriptoresByMessageQueueId(uint32_t id)
{
    t_message_queue* messageQueue = getMessageQueueById(id);
    t_list* suscriptores = list_create();
	pthread_mutex_lock(&messageQueue->s_subscribers);
		for(uint32_t i = 0; i < list_size(messageQueue->subscribers); i++)
		{
			list_add(suscriptores, list_get(messageQueue->subscribers,i));
		}
	pthread_mutex_unlock(&messageQueue->s_subscribers);

    return suscriptores;
}

void processMessage(t_buffer *buffer, uint32_t operation_cod, uint32_t socket_cliente) 
{
	switch (operation_cod)
	{
		case SUBSCRIBE:
		{
			log_info(logger, "Se ha conectado un proceso a traves del socket %d", socket_cliente);
			t_register_module* registerModule = deserializar_registerModule(buffer);
			subscribeNewModule(registerModule->moduleId, socket_cliente, registerModule->messageQueue);
			free(registerModule);
			break;
		}
		case ACKNOWLEDGEMENT:
		{
			t_acknowledgement* ack = deserializar_acknowledgement(buffer);
			receiveAcknowledgement(ack, socket_cliente);
			//log_info(broker_custom_logger, "Se ha recibido ACK de %i por el mensaje %i", socket_cliente, ack->idMessageReceived);
			break;
		}
		case UNSUBSCRIBE:
		{
			t_register_module* unregisterModule = deserializar_registerModule(buffer);
			unsubscribeModule(unregisterModule->moduleId, socket_cliente, unregisterModule->messageQueue);
			free(unregisterModule);
			break;
		}
		case NEW_POKEMON:
		{
			log_info(logger, "Se ha conectado un proceso a traves del socket %d", socket_cliente);
			t_message_queue* messageQueue = getMessageQueueById(operation_cod);
			t_new_pokemon* newPoke = deserializar_newPokemon(buffer);
			t_message* message = crearMessage();

			message->id = asignarMessageId();
			message->idCorrelativo = 0;
			message->mq_cod = NEW_POKEMON;
			newPoke->ID_mensaje_recibido = message->id;
			message->mensaje = newPoke;
			log_info(logger, "Se ha recibido un mensaje del cliente %i en la cola %s", socket_cliente, messageQueue->name);
			addMessageToQueue(message, messageQueue);

			notifySender(message, socket_cliente);	
			break;
		}
		case APPEARED_POKEMON:
		{
			log_info(logger, "Se ha conectado un proceso a traves del socket %d", socket_cliente);
			t_message_queue* messageQueue = getMessageQueueById(operation_cod);
			t_appeared_pokemon* appearedPoke = deserializar_appearedPokemon(buffer);
			t_message* message = crearMessage(appearedPoke, APPEARED_POKEMON);

			message->id = asignarMessageId();
			message->idCorrelativo = appearedPoke->ID_mensaje_original;
			message->mq_cod = APPEARED_POKEMON;
			appearedPoke->ID_mensaje_recibido = message->id;
			message->mensaje = appearedPoke;

			log_info(logger, "Se ha recibido un mensaje del cliente %i en la cola %s", socket_cliente, messageQueue->name);
			addMessageToQueue(message, messageQueue);
			
			notifySender(message, socket_cliente);	
			break;
		}
		case GET_POKEMON:
		{
			log_info(logger, "Se ha conectado un proceso a traves del socket %d", socket_cliente);
			t_message_queue* messageQueue = getMessageQueueById(operation_cod);
			t_get_pokemon* getPoke = deserializar_getPokemon(buffer);
			t_message* message = crearMessage(getPoke, GET_POKEMON);

			message->id = asignarMessageId();
			message->idCorrelativo = 0;
			message->mq_cod = GET_POKEMON;
			getPoke->ID_mensaje_recibido = message->id;
			message->mensaje = getPoke;

			log_info(logger, "Se ha recibido un mensaje del cliente %i en la cola %s", socket_cliente, messageQueue->name);
			addMessageToQueue(message, messageQueue);
			
			notifySender(message, socket_cliente);	
			break;
		}
		case CAUGHT_POKEMON:
		{
			log_info(logger, "Se ha conectado un proceso a traves del socket %d", socket_cliente);
			t_message_queue* messageQueue = getMessageQueueById(operation_cod);
			t_caught_pokemon* caughtPoke = deserializar_caughtPokemon(buffer);
			t_message* message = crearMessage(caughtPoke, CAUGHT_POKEMON);

			message->id = asignarMessageId();
			message->idCorrelativo = caughtPoke->ID_mensaje_original;
			message->mq_cod = CAUGHT_POKEMON;
			caughtPoke->ID_mensaje_recibido = message->id;
			message->mensaje = caughtPoke;
			
			log_info(logger, "Se ha recibido un mensaje del cliente %i en la cola %s", socket_cliente, messageQueue->name);
			addMessageToQueue(message, messageQueue);
			
			notifySender(message, socket_cliente);	
			break;
		}
		case LOCALIZED_POKEMON:
		{
			log_info(logger, "Se ha conectado un proceso a traves del socket %d", socket_cliente);
			t_message_queue* messageQueue = getMessageQueueById(operation_cod);
			t_localized_pokemon* localizedPoke = deserializar_localizedPokemon(buffer);
			t_message* message = crearMessage(localizedPoke, LOCALIZED_POKEMON);

			message->id = asignarMessageId();
			message->idCorrelativo = localizedPoke->ID_mensaje_original;
			message->mq_cod = LOCALIZED_POKEMON;
			localizedPoke->ID_mensaje_recibido = message->id;
			message->mensaje = localizedPoke;

			log_info(logger, "Se ha recibido un mensaje del cliente %i en la cola %s", socket_cliente, messageQueue->name);
			addMessageToQueue(message, messageQueue);
			
			notifySender(message, socket_cliente);	
			break;
		}
		case CATCH_POKEMON:
		{
			log_info(logger, "Se ha conectado un proceso a traves del socket %d", socket_cliente);
			t_message_queue* messageQueue = getMessageQueueById(operation_cod);
			t_catch_pokemon* catchPoke = deserializar_catchPokemon(buffer);
			t_message* message = crearMessage(catchPoke, CATCH_POKEMON);

			message->id = asignarMessageId();
			message->idCorrelativo = 0;
			message->mq_cod = CATCH_POKEMON;
			catchPoke->ID_mensaje_recibido = message->id;
			message->mensaje = catchPoke;

			log_info(logger, "Se ha recibido un mensaje del cliente %i en la cola %s", socket_cliente, messageQueue->name);
			addMessageToQueue(message, messageQueue);
			
			notifySender(message, socket_cliente);	
			break;
		}
		default:
			log_error(broker_custom_logger, "Codigo de operacion desconocido: %i", operation_cod);
	}
}

void receiveAcknowledgement(t_acknowledgement* ack, uint32_t socket_cliente)
{
	t_message_queue* messageQueue = getMessageQueueById(ack->mq);

	t_message* targetMessage;
	t_message* currentMessage;

	pthread_mutex_lock(&messageQueue->s_mensajes);
	for(uint32_t i = 0; i < list_size(messageQueue->mensajes); i++) {
		currentMessage = (t_message*)list_get(messageQueue->mensajes, i);
		if (currentMessage->id == ack->idMessageReceived) {
			targetMessage = currentMessage;
			break;
		}
	}
	pthread_mutex_unlock(&messageQueue->s_mensajes);

	if(targetMessage != NULL)
	{
		pthread_mutex_lock(&targetMessage->s_suscriptoresConfirmados);
		pthread_mutex_lock(&targetMessage->s_suscriptoresEnviados);
			t_suscripcion* targetSuscriptor;
			uint32_t _is_the_subscriber(t_suscripcion* suscripcion) {
				return suscripcion->idModule == ack->idModule;
			}
				targetSuscriptor = (t_suscripcion*)list_find(targetMessage->suscriptoresEnviados, (void*)_is_the_subscriber);
			if(targetSuscriptor != NULL)
				list_add(targetMessage->suscriptoresConfirmados, targetSuscriptor);

			uint32_t countSuscriptoresEnviados = list_size(targetMessage->suscriptoresEnviados);
			uint32_t countSuscriptoresConfirmados = list_size(targetMessage->suscriptoresConfirmados);		
			log_info(broker_custom_logger, "Recibido ack del cliente id %i por el mensaje id %i", targetSuscriptor->idModule , targetMessage->id);
			
			if(countSuscriptoresEnviados == countSuscriptoresConfirmados && countSuscriptoresConfirmados == targetMessage->countSuscriptoresObjetivo)
				sem_post(&targetMessage->s_puedeEliminarse);
				
		pthread_mutex_unlock(&targetMessage->s_suscriptoresEnviados);
		pthread_mutex_unlock(&targetMessage->s_suscriptoresConfirmados);
	}
	else
	{
		//log_debug(broker_custom_logger, "Mensaje con id %i no encontrado en la cola %s", ack->idMessageReceived, messageQueue->name);
	}
}

void subscribeNewModule(uint32_t idNewModule, uint32_t socket_cliente, uint32_t mq_cod)
{
	t_message_queue* messageQueue = getMessageQueueById(mq_cod);
	t_suscripcion* suscripcion = crearSuscripcion(idNewModule, socket_cliente);

	t_suscripcion* foundSuscripcion;
	pthread_mutex_lock(&messageQueue->s_subscribers);
	uint32_t _it_was_already_register(t_suscripcion* suscripcion) {
		return suscripcion->idModule == idNewModule;
	}
	foundSuscripcion = (t_suscripcion*)list_find(messageQueue->subscribers, (void*)_it_was_already_register);
	if(foundSuscripcion != NULL)
	{
		//Se actualiza su socket de conexion si es necesario
		if(foundSuscripcion->socket != suscripcion->socket)
		{
			foundSuscripcion->socket = suscripcion->socket;
		}
		log_info(logger, "El suscriptor id %i se ha vuelto a conectar a la cola %s", foundSuscripcion->idModule, messageQueue->name);
	}
	else 
	{
		list_add(messageQueue->subscribers, (void *)suscripcion);
		log_info(logger, "Se ha registrado un nuevo suscriptor id %i a la cola %s", suscripcion->idModule, messageQueue->name);
	}
	pthread_mutex_unlock(&messageQueue->s_subscribers);

	//Enviar los mensajes de la cache
	t_cache_dispatch_info* cache_dispatch_info = (t_cache_dispatch_info*)malloc(sizeof(t_cache_dispatch_info));
	cache_dispatch_info->suscripcion = suscripcion;
	cache_dispatch_info->mq_cod = mq_cod;

	pthread_t dispatchCache;
	pthread_create(&dispatchCache, NULL, (void*)dispatchCachedMessages, cache_dispatch_info);
	pthread_join(dispatchCache, NULL);
}

void unsubscribeModule(uint32_t idNewModule, uint32_t socket_cliente, uint32_t mq_cod)
{
	t_message_queue* messageQueue = getMessageQueueById(mq_cod);

	t_suscripcion* foundSuscripcion;
	pthread_mutex_lock(&messageQueue->s_subscribers);
	uint32_t _is_the_registration(t_suscripcion* suscripcion) {
		return suscripcion->idModule == idNewModule;
	}
	foundSuscripcion = (t_suscripcion*)list_find(messageQueue->subscribers, (void*)_is_the_registration);

	if(foundSuscripcion == NULL)
	{
		log_debug(broker_custom_logger, "founSubscription not found");
		return;
	}
	
	pthread_mutex_lock(&messageQueue->s_mensajes);
	t_suscripcion* targetSuscription;
	for(uint32_t i = 0; i < list_size(messageQueue->subscribers); i++)
	{
		targetSuscription = (t_suscripcion*)list_get(messageQueue->subscribers, i);
		if(targetSuscription->idModule == foundSuscripcion->idModule)
		{
			log_debug(broker_custom_logger, "Se ha desuscripto el modulo id %i con el socket %i de la cola %s", 
				targetSuscription->idModule, targetSuscription->socket, messageQueue->name);
			shutdown(targetSuscription->socket, 2);
			close(targetSuscription->socket);
			list_remove(messageQueue->subscribers, i);
			break;
		}
	}
	t_message* currentMessage;
	for(uint32_t i = 0; i < list_size(messageQueue->mensajes); i++)
	{
		currentMessage = (t_message*)list_get(messageQueue->mensajes, i);
		currentMessage->countSuscriptoresObjetivo--;
	}
	pthread_mutex_unlock(&messageQueue->s_mensajes);
	pthread_mutex_unlock(&messageQueue->s_subscribers);
}

void addMessageToQueue(t_message* message, t_message_queue* messageQueue)
{
	pthread_mutex_lock(&messageQueue->s_mensajes);
		list_add(messageQueue->mensajes, (void*)message);
		sem_post(&messageQueue->s_hayMensajes);
	pthread_mutex_unlock(&messageQueue->s_mensajes);
}

void sendMessageFromQueue(t_message* message, t_suscripcion* suscriptor)
{
	t_paquete* paquete = (t_paquete*)malloc(sizeof(t_paquete));
	switch(message->mq_cod)
	{
		case NEW_POKEMON:
		{
			t_new_pokemon* newPoke;
			newPoke = (t_new_pokemon*)message->mensaje;
			paquete = serializar_newPokemon(newPoke);
			break;
		}
		case APPEARED_POKEMON:
		{
			t_appeared_pokemon* appearedPoke;
			appearedPoke = (t_appeared_pokemon*)message->mensaje;
			paquete = serializar_appearedPokemon(appearedPoke);
			break;
		}
		case CATCH_POKEMON:
		{
			t_catch_pokemon* catchPoke;
			catchPoke = (t_catch_pokemon*)message->mensaje;
			paquete = serializar_catchPokemon(catchPoke);
			break;
		}
		case CAUGHT_POKEMON:
		{
			t_caught_pokemon* caughtPoke;
			caughtPoke = (t_caught_pokemon*)message->mensaje;
			paquete = serializar_caughtPokemon(caughtPoke);
			break;
		}
		case GET_POKEMON:
		{
			t_get_pokemon* getPoke;
			getPoke = (t_get_pokemon*)message->mensaje;
			paquete = serializar_getPokemon(getPoke);
			break;
		}
		case LOCALIZED_POKEMON:
		{
			t_localized_pokemon* localizedPoke;
			localizedPoke = (t_localized_pokemon*)message->mensaje;
			paquete = serializar_localizedPokemon(localizedPoke);
			break;
		}
	}

	uint32_t sendResult = enviarMensaje_returnResult(paquete, suscriptor->socket);
	if(sendResult == 1)
	{
		//Que pasa si falla el send? A veces no falla, pero por debajo falló
	}
	else if (sendResult == 0)
	{
		pthread_mutex_lock(&message->s_suscriptoresEnviados);
			list_add(message->suscriptoresEnviados, suscriptor);
		pthread_mutex_unlock(&message->s_suscriptoresEnviados);
		log_info(logger, "Mensaje id %i enviado a suscriptor %i", message->id, suscriptor->idModule);
	}
}

void dispatchMessagesFromQueue(t_message_queue* messageQueue)
{
	t_suscripcion* currentSubscriber;
	uint32_t subscribersCount;
	while(true)
	{
		sem_wait(&messageQueue->s_hayMensajes);
		t_message* message;
		for(uint32_t i = 0; i < list_size(messageQueue->mensajes); i++)
		{
			message = (t_message*)list_get(messageQueue->mensajes, i);
			pthread_mutex_lock(&messageQueue->s_subscribers);
				subscribersCount = list_size(messageQueue->subscribers);
				if(subscribersCount == 0)
				{
					sem_post(&message->s_puedeEliminarse);
				}
				message->countSuscriptoresObjetivo = subscribersCount;
				for(uint32_t i = 0; i < subscribersCount; i++) {
					currentSubscriber = (t_suscripcion*)list_get(messageQueue->subscribers, i);
					sendMessageFromQueue(message, currentSubscriber);
				}
			pthread_mutex_unlock(&messageQueue->s_subscribers);	
			
			//Guardarlo en la cache
			if(pthread_create(&message->caching, NULL, (void*)cacheMessage, message) < 0)
				log_error(broker_custom_logger, "Error in pthread_create cacheMessage");
			
			if(pthread_detach(message->caching) != 0)
				log_error(broker_custom_logger, "Error in pthread_join cacheMessage");		
			
			//Eliminarlo de la cola
			if(pthread_create(&message->deleteFromQueue, NULL, (void*)deleteFromQueue, message) < 0)
				log_error(broker_custom_logger, "Error in pthread_create deleteFromQueue");
			
			if(pthread_join(message->deleteFromQueue, NULL) != 0)
				log_error(broker_custom_logger, "Error in pthread_detach deleteFromQueue");
		}
		pthread_mutex_unlock(&messageQueue->s_mensajes);
	}
}

void deleteFromQueue(t_message* message) 
{
	sem_wait(&message->s_puedeEliminarse); //Cache
	sem_wait(&message->s_puedeEliminarse); //Ack
	t_message_queue* messageQueue = getMessageQueueById(message->mq_cod);
	t_message* current = (t_message*)malloc(sizeof(t_message));;
	uint32_t targetIndex;
	pthread_mutex_lock(&messageQueue->s_mensajes);
		for(uint32_t i = 0; i < list_size(messageQueue->mensajes); i++)
		{
			current = list_get(messageQueue->mensajes, i);
			if(current->id == message->id)
			{
				targetIndex = i;
				break;
			}
		}
		if(targetIndex >= 0)
		{
			list_remove(messageQueue->mensajes, targetIndex);			
			//log_debug(broker_custom_logger, "Mensaje eliminado de la cola");
			free(current);
		}
	pthread_mutex_unlock(&messageQueue->s_mensajes);
}

void notifySender(t_message* message, uint32_t socket_cliente)
{
	//Se comunica el id al emisor
	t_id_mensaje_recibido* idAsignado = crearIdMensajeRecibido(message->id);		
	t_paquete* paquete = serializar_idMensajeRecibido(idAsignado);
	enviarMensaje(paquete, socket_cliente);
	free(idAsignado);
	//log_info(broker_custom_logger, "Respuesta de id (%i) enviada al cliente %i", message->id, socket_cliente);		
}

void cacheMessage(t_message* message)
{
	algoritmo_memoria(message);
	sem_post(&message->s_puedeEliminarse);
	//log_debug(logger, "Mensaje id %i agregado a la cache", message->id);
}

void inicializarCounterMessageId()
{
    globalMessageCounterId = 0;
	return;
}

uint32_t asignarMessageId()
{
	pthread_mutex_lock(&s_counterMessageId);
		globalMessageCounterId++;
	pthread_mutex_unlock(&s_counterMessageId);
	
    return globalMessageCounterId;
}

//Cleaning
void freeMessage(t_message* message) 
{
	list_destroy(message->suscriptoresEnviados);
	list_destroy(message->suscriptoresConfirmados);
	free(message->mensaje);
	free(message);
}

void freeMessageQueue(t_message_queue* messageQueue)
{
	list_destroy_and_destroy_elements(messageQueue->mensajes, (void*)freeMessage);
	list_destroy(messageQueue->subscribers);
	free(messageQueue);
}