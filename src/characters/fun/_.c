#include "prefabs.c"

static inline entity get_player_character(ecs *world, entity player) {
    return zox_get_link(world, player, CharacterLink);
}

entity get_linked_character(ecs* world, entity e) {
    return zox_get_link(world, e, CharacterLink);
}
