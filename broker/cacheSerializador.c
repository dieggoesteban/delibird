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

    log_debug(broker_custom_logger, "Serializar: %s", string_substring(newPokemon->pokeName, 0, 7));
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
	memcpy(newPokemon->pokeName, stream, newPokemon->nameLength + 1);
	stream += newPokemon->nameLength + 1;
    string_append(&newPokemon->pokeName, "\0");

	return newPokemon;
}