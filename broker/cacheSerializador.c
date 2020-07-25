#include "cacheSerializador.h"

t_cache_buffer* serializar_cacheNewPokemon(cache_new_pokemon* newPokemon)
{
    t_cache_buffer* newPokemon_buffer = malloc(sizeof(t_cache_buffer));
    newPokemon_buffer->size = (sizeof(uint32_t) * 4 + (newPokemon->nameLength));

    void* stream = malloc(newPokemon_buffer->size);
    uint32_t offset = 0;

    memcpy(stream + offset, &newPokemon->nameLength, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &newPokemon->posX, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &newPokemon->posY, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &newPokemon->cantidad, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, newPokemon->pokeName, newPokemon->nameLength);
    offset += newPokemon->nameLength;

    newPokemon_buffer->stream = stream;

	return newPokemon_buffer;
}
cache_new_pokemon* deserializar_cacheNewPokemon(t_cache_buffer* buffer)
{
    cache_new_pokemon* newPokemon = (cache_new_pokemon*)malloc(sizeof(cache_new_pokemon));
    
	void* stream = buffer->stream;

	memcpy(&(newPokemon->nameLength), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	memcpy(&(newPokemon->posX), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	memcpy(&(newPokemon->posY), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&(newPokemon->cantidad), stream, sizeof(uint32_t));
    stream += sizeof(u_int32_t);
	newPokemon->pokeName = malloc(newPokemon->nameLength + 1);
	memcpy(newPokemon->pokeName, stream, newPokemon->nameLength);
	stream += newPokemon->nameLength + 1;
    strcpy(newPokemon->pokeName + newPokemon->nameLength, "\0");

	return newPokemon;
}

t_cache_buffer* serializar_cacheAppearedPokemon(cache_appeared_pokemon* appearedPokemon)
{
    t_cache_buffer* appearedPokemon_buffer = malloc(sizeof(t_cache_buffer));
    appearedPokemon_buffer->size = (sizeof(uint32_t) * 3 + (appearedPokemon->nameLength));

    void* stream = malloc(appearedPokemon_buffer->size);
    uint32_t offset = 0; 

    memcpy(stream + offset, &appearedPokemon->nameLength, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &appearedPokemon->posX, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &appearedPokemon->posY, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, appearedPokemon->pokeName, appearedPokemon->nameLength);
    offset += appearedPokemon->nameLength;

    appearedPokemon_buffer->stream = stream;

	return appearedPokemon_buffer;
}
cache_appeared_pokemon* deserializar_cacheAppearedPokemon(t_cache_buffer* buffer)
{
    cache_appeared_pokemon* appearedPokemon = (cache_appeared_pokemon*)malloc(sizeof(cache_appeared_pokemon));
    
	void* stream = buffer->stream;

	memcpy(&(appearedPokemon->nameLength), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	memcpy(&(appearedPokemon->posX), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	memcpy(&(appearedPokemon->posY), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
	appearedPokemon->pokeName = malloc(appearedPokemon->nameLength + 1);
	memcpy(appearedPokemon->pokeName, stream, appearedPokemon->nameLength);
	stream += appearedPokemon->nameLength + 1;
    strcpy(appearedPokemon->pokeName + appearedPokemon->nameLength, "\0");

	return appearedPokemon;
}

t_cache_buffer* serializar_cacheCatchPokemon(cache_catch_pokemon* catchPokemon)
{
    t_cache_buffer* catchPokemon_buffer = malloc(sizeof(t_cache_buffer));
    catchPokemon_buffer->size = (sizeof(uint32_t) * 3 + (catchPokemon->nameLength));

    void* stream = malloc(catchPokemon_buffer->size);
    uint32_t offset = 0;

    memcpy(stream + offset, &catchPokemon->nameLength, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &catchPokemon->posX, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &catchPokemon->posY, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, catchPokemon->pokeName, catchPokemon->nameLength);
    offset += catchPokemon->nameLength;

    catchPokemon_buffer->stream = stream;

	return catchPokemon_buffer;
}
cache_catch_pokemon* deserializar_cacheCatchPokemon(t_cache_buffer* buffer)
{
    cache_catch_pokemon* catchPokemon = (cache_catch_pokemon*)malloc(sizeof(cache_catch_pokemon));
    
	void* stream = buffer->stream;

	memcpy(&(catchPokemon->nameLength), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	memcpy(&(catchPokemon->posX), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	memcpy(&(catchPokemon->posY), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
	catchPokemon->pokeName = malloc(catchPokemon->nameLength + 1);
	memcpy(catchPokemon->pokeName, stream, catchPokemon->nameLength);
	stream += catchPokemon->nameLength + 1;
    strcpy(catchPokemon->pokeName + catchPokemon->nameLength, "\0");

	return catchPokemon;
}

t_cache_buffer* serializar_cacheCaughtPokemon(cache_caught_pokemon* caughtPokemon)
{
    t_cache_buffer* caughtPokemon_buffer = malloc(sizeof(t_cache_buffer));
    caughtPokemon_buffer->size = (sizeof(uint32_t));

    void* stream = malloc(caughtPokemon_buffer->size);
    uint32_t offset = 0;

    memcpy(stream + offset, &caughtPokemon->seAtrapo, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    caughtPokemon_buffer->stream = stream;

	return caughtPokemon_buffer;
}
cache_caught_pokemon* deserializar_cacheCaughtPokemon(t_cache_buffer* buffer)
{
    cache_caught_pokemon* caughtPokemon = (cache_caught_pokemon*)malloc(sizeof(cache_caught_pokemon));
    
	void* stream = buffer->stream;

	memcpy(&(caughtPokemon->seAtrapo), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);

	return caughtPokemon;
}

t_cache_buffer* serializar_cacheGetPokemon(cache_get_pokemon* getPokemon)
{
    t_cache_buffer* getPokemon_buffer = malloc(sizeof(t_cache_buffer));
    getPokemon_buffer->size = (sizeof(uint32_t) + (getPokemon->nameLength));

    void* stream = malloc(getPokemon_buffer->size);
    uint32_t offset = 0;

    memcpy(stream + offset, &getPokemon->nameLength, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, getPokemon->pokeName, getPokemon->nameLength);
    offset += getPokemon->nameLength;

    getPokemon_buffer->stream = stream;

	return getPokemon_buffer;
}
cache_get_pokemon* deserializar_cacheGetPokemon(t_cache_buffer* buffer)
{
    cache_get_pokemon* getPokemon = (cache_get_pokemon*)malloc(sizeof(cache_get_pokemon));
    
	void* stream = buffer->stream;

	memcpy(&(getPokemon->nameLength), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	getPokemon->pokeName = malloc(getPokemon->nameLength + 1);
	memcpy(getPokemon->pokeName, stream, getPokemon->nameLength);
	stream += getPokemon->nameLength + 1;
    strcpy(getPokemon->pokeName + getPokemon->nameLength, "\0");

	return getPokemon;
}

t_cache_buffer* serializar_cacheLocalizedPokemon(cache_localized_pokemon* localizedPokemon)
{
    t_cache_buffer* localizedPokemon_buffer = malloc(sizeof(t_cache_buffer));
    // getPokemon_buffer->size = (sizeof(uint32_t) + (getPokemon->nameLength));

    // void* stream = malloc(getPokemon_buffer->size);
    // uint32_t offset = 0;

    // memcpy(stream + offset, &getPokemon->nameLength, sizeof(uint32_t));
    // offset += sizeof(uint32_t);
    // memcpy(stream + offset, getPokemon->pokeName, getPokemon->nameLength);
    // offset += getPokemon->nameLength;

    //localizedPokemon_buffer->stream = stream;

	return localizedPokemon_buffer;
}
cache_localized_pokemon* deserializar_cacheLocalizedPokemon(t_cache_buffer* buffer)
{
    cache_localized_pokemon* localizedPokemon = (cache_localized_pokemon*)malloc(sizeof(cache_localized_pokemon));

	//void* stream = buffer->stream;
	// memcpy(&(getPokemon->nameLength), stream, sizeof(uint32_t));
	// stream += sizeof(uint32_t);
	// getPokemon->pokeName = malloc(getPokemon->nameLength + 1);
	// memcpy(getPokemon->pokeName, stream, getPokemon->nameLength + 1);
	// stream += getPokemon->nameLength + 1;
    // string_append(&getPokemon->pokeName, "\0");

	return localizedPokemon;
}

t_new_pokemon* newPokemon_cacheToMessage(cache_new_pokemon* cacheNewPokemon, cache_message* metadata)
{
    t_posicion_cantidad* posCant = crearPosicionCantidad(cacheNewPokemon->posX, cacheNewPokemon->posY, cacheNewPokemon->cantidad);
    t_new_pokemon* newPokemon = crearNewPokemon(metadata->idMessage, cacheNewPokemon->pokeName, posCant);
    return newPokemon;
}

t_appeared_pokemon* appearedPokemon_cacheToMessage(cache_appeared_pokemon* cacheAppearedPokemon, cache_message* metadata)
{
    t_posicion* pos = crearPosicion(cacheAppearedPokemon->posX, cacheAppearedPokemon->posY);
    t_appeared_pokemon* appearedPokemon = crearAppearedPokemon(metadata->idMessage, cacheAppearedPokemon->pokeName, pos);
    return appearedPokemon;
}
t_catch_pokemon* catchPokemon_cacheToMessage(cache_catch_pokemon* cacheCatchPokemon, cache_message* metadata)
{
    t_posicion* pos = crearPosicion(cacheCatchPokemon->posX, cacheCatchPokemon->posY);
    t_catch_pokemon* catchPokemon = crearCatchPokemon(metadata->idMessage, cacheCatchPokemon->pokeName, pos);
    return catchPokemon;
}
t_caught_pokemon* caughtPokemon_cacheToMessage(cache_caught_pokemon* cacheCaughtPokemon, cache_message* metadata)
{
    t_caught_pokemon* caughtPokemon = crearCaughtPokemon(metadata->idMessage, metadata->idCorrelational, cacheCaughtPokemon->seAtrapo);
    return caughtPokemon;
}
t_get_pokemon* getPokemon_cacheToMessage(cache_get_pokemon* cacheGetPokemon, cache_message* metadata)
{
    t_get_pokemon* getPokemon = crearGetPokemon(metadata->idMessage, cacheGetPokemon->pokeName);
    return getPokemon;
}
//t_localized_pokemon* localizedPokemon_cacheToMessage(cache_localized_pokemon* cacheLocalizedPokemon, cache_message* metadata);