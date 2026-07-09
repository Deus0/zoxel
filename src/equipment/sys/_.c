#include "character.c"

void define_systems_equipment(ecs* world) {
    zox_system_1(
        CharacterPlayerEquipsSystem,
        zoxp_mainthread,
        [in] characters.GenerateCharacter,
        [in] realms.RealmLink,
        [none] players.PlayerLink
    );
}
