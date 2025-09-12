#include "prefabs.c"

static inline entity get_player_character(ecs *world, entity p) {
    return zox_valid(p) && zox_has(p, CharacterLink) ? zox_gett_value(p, CharacterLink) : 0;
}

entity get_linked_character(ecs* world, const entity e) {
    return zox_valid(e) && zox_has(e, CharacterLink) ? zox_gett_value(e, CharacterLink) : 0;
}