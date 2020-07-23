#include "global-includes.h"
#include "models.h"

typedef struct
{
    uint32_t size;
    void* stream;
} t_cache_buffer;

t_cache_buffer* serializar_cacheNewPokemon(cache_new_pokemon* newPokemon);
cache_new_pokemon* deserializar_cacheNewPokemon(t_cache_buffer* buffer);

