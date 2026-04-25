#include "character.c"

void define_systems_equipment(ecs* world) {

    zox_system_1(
        CharacterPlayerEquipsSystem,
        EcsOnUpdate,
        [in] characters.GenerateCharacter,
        [in] realms.RealmLink,
        [out] equips.EquipLinks,
        [none] players.PlayerLink
    );
}
