#include "character.c"
#include "slay.c"
realm_clear_system(QuestLinks);

void define_systems_quests(ecs* world) {
    realm_clear_systemd(quests, QuestLinks);
    zox_system_1(
        CharacterPlayerQuestsSystem,
        zoxp_mainthread,
        [in] characters.GenerateCharacter,
        [in] realms.RealmLink,
        [none] characters.Character,
        [none] players.PlayerLink
    );
    zox_system(
        SlaySystem,
        EcsOnUpdate,
        [in] combat.Dead,
        [in] combat.LastDamager
    );
}
