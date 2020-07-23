#include "global-includes.h"
#include "models.h"
#include "serializador.h"

typedef struct
{
    uint32_t size;
    void* stream;
} t_cache_buffer;

t_cache_buffer* serializar_cacheNewPokemon(cache_new_pokemon* newPokemon);
cache_new_pokemon* deserializar_cacheNewPokemon(t_cache_buffer* buffer);

t_cache_buffer* serializar_cacheAppearedPokemon(cache_appeared_pokemon* appearedPokemon);
cache_appeared_pokemon* deserializar_cacheAppearedPokemon(t_cache_buffer* buffer);

t_cache_buffer* serializar_cacheCatchPokemon(cache_catch_pokemon* catchPokemon);
cache_catch_pokemon* deserializar_cacheCatchPokemon(t_cache_buffer* buffer);

t_cache_buffer* serializar_cacheCaughtPokemon(cache_caught_pokemon* caughtPokemon);
cache_caught_pokemon* deserializar_cacheCaughtPokemon(t_cache_buffer* buffer);

t_cache_buffer* serializar_cacheGetPokemon(cache_get_pokemon* getPokemon);
cache_get_pokemon* deserializar_cacheGetPokemon(t_cache_buffer* buffer);

t_cache_buffer* serializar_cacheLocalizedPokemon(cache_localized_pokemon* localizedPokemon);
cache_localized_pokemon* deserializar_cacheLocalizedPokemon(t_cache_buffer* buffer);

t_new_pokemon* newPokemon_cacheToMessage(cache_new_pokemon* cacheNewPokemon, cache_message* metadata);