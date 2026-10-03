#include "character.c"
// #include "slay.c"
#include "marker.c"

void zox_systems_quests(ecs* world) {
    zox_system_1(
        CharacterPlayerQuestsSystem,
        zoxp_spawn,
        [in] characters.GenerateCharacter,
        [none] characters.Character,
        [none] players.PlayerCharacter,
    );
    zox_system_1(
        marker_spawn_system,
        zoxp_spawn,
        [in] characters.GenerateCharacter,
        [out] ui.ElementLinks,
        [none] characters.Character,
    );
}
