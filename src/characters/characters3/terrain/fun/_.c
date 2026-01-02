#include "log.c"

static inline byte can_have_characters(ecs* world, entity chunk) {
    return zox_get_value(chunk, CharacterSpawnZone);
}