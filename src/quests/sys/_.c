#include "character.c"
#include "slay.c"
realm_clear_system(QuestLinks);

void define_systems_quests(ecs* world) {

    realm_clear_systemd(quests, QuestLinks);

    zox_system_1(
        CharacterPlayerQuestsSystem,
        EcsOnUpdate,
        [in] characters.GenerateCharacter,
        [in] realms.RealmLink,
        [out] quests.QuestLinks,
        [none] players.PlayerLink
    );

    zox_system(
        SlaySystem,
        EcsOnUpdate,
        [in] combat.Dead,
        [in] combat.LastDamager
    );
}
