#include "character.c"
// #include "slay.c"
#include "marker.c"

void zox_systems_quests(ecs* world) {
    /*zox_system(
        SlaySystem,
        zoxp_update,
        [none] characters.Character,
        [none] core.DeathDirty,
    );*/
    zox_system_1(
        CharacterPlayerQuestsSystem,
        zoxp_spawn,
        [in] characters.GenerateCharacter,
        [none] characters.Character,
        [none] players.PlayerCharacter
    );
    zox_system_1(
        MarkerSpawnSystem,
        zoxp_spawn,
        [in] characters.GenerateCharacter,
        [out] ui.ElementLinks,
        [none] characters.Character,
    );
}
