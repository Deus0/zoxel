#include "character.c"

void zox_systems_equipment(ecs* world) {
    zox_system_1(
        CharacterPlayerEquipsSystem,
        zoxp_spawn,
        [in] characters.GenerateCharacter,
        [out] parts.BodyDirty,
        [none] players.PlayerCharacter
    );
}
