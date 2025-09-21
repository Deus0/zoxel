#include "realm.c"
#include "character.c"
#include "slay.c"

zox_declare_system_state_event(RealmQuests, GenerateRealm, zox_generate_realm_quests, spawn_realm_quests)

void define_systems_quests(ecs* world) {
    zox_define_system_state_event_1(RealmQuests, EcsOnLoad, realms.GenerateRealm, [none] realms.Realm);

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