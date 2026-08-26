#include "character.c"

void define_systems_equipment(ecs* world) {
    zox_system_1(
        CharacterPlayerEquipsSystem,
        zoxp_spawn,
        [in] characters.GenerateCharacter,
        [in] realms.RealmLink,
        [out] parts.BodyDirty,
        [none] players.PlayerLink
    );
}
