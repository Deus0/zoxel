#include "log.c"

static inline byte can_have_characters(ecs_world_t* world, ecs_entity_t chunk) {
    return zox_get_value(chunk, CharacterSpawnZone);
}