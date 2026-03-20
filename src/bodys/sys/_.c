#include "realm.c"
#include "character.c"
#include "combine.c"

void define_systems_bodys(ecs* world) {

    zox_system_1(
        BodysRealmSpawnSystem,
        EcsOnUpdate,
        [in] realms.GenerateRealm,
        [out] rendering.ModelLinks,
        [out] items.ItemLinks,
        [out] nodes.NodegraphLinks,
        [none] realms.Realm
    );

    zox_system_1(
        CharacterBodySpawnSystem,
        EcsOnUpdate,
        [in] characters.GenerateCharacter,
        [in] realms.RealmLink,
        [out] bodys.BodyLinks,
        [out] bones.BoneLinks,
        [out] bodys.BodyDirty,
        [none] players.PlayerCharacter
    );

    zox_system(
        BodyCombineSystem,
        EcsOnUpdate,
        [in] bodys.BodyDirty,
        [in] bodys.BodyLinks,
        [out] voxes.CombineList,
        [out] voxes.CombinePositions,
        [out] voxes.CombineVox,
        [none] players.PlayerCharacter
    );
}
