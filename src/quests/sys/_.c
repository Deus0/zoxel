#include "character.c"
#include "slay.c"

void define_systems_quests(ecs* world) {
    zox_system(
        SlaySystem,
        zoxp_update,
        [in] combat.Dead,
        [in] combat.LastDamager
    );
    zox_system_1(
        CharacterPlayerQuestsSystem,
        zoxp_spawn,
        [in] characters.GenerateCharacter,
        [in] realms.RealmLink,
        [none] characters.Character,
        [none] players.PlayerLink
    );
}
