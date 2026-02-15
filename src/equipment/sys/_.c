#include "character.c"
#include "realm.c"

void define_systems_equipment(ecs* world) {
    zox_system_1(
        EquipsRealmSpawnSystem,
        EcsOnUpdate,
        [in] realms.GenerateRealm,
        [in] rendering.ModelLinks,
        [out] items.ItemLinks,
        [none] realms.Realm
    );

    zox_system_1(
        CharacterPlayerEquipsSystem,
        EcsOnUpdate,
        [in] characters.GenerateCharacter,
        [in] realms.RealmLink,
        [out] equips.EquipLinks,
        [none] players.PlayerLink
    );
}
