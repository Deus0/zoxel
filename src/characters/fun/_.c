#include "prefabs.c"

static inline entity get_player_character(ecs *world, entity p) {
    return zox_valid(p) && zox_has(p, CharacterLink) ? zox_getv(p, CharacterLink) : 0;
}

entity get_linked_character(ecs* world, entity e) {
    return zox_valid(e) && zox_has(e, CharacterLink) ? zox_getv(e, CharacterLink) : 0;
}
