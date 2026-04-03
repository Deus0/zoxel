#include "realm.c"
#include "character.c"
#include "combine.c"
#include "bones.c"

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
        [out] bodys.BodySize,
        [out] bodys.PartLinks,
        [out] bodys.BodyDirty,
        [out] blocks.BlockScale,
        [none] players.PlayerCharacter
    );

    zox_system_1(
        CharacterBoneSpawnSystem,
        EcsOnUpdate,
        [in] bodys.BodyDirty,
        [in] bodys.PartLinks,
        [out] hierarchys.Children,
        [out] bones.BoneLinks,
        [out] bones.SkeletonDirty,
        [none] bones.Skeleton
    );

    zox_system(
        BodyCombineSystem,
        EcsOnUpdate,
        [in] bodys.BodyDirty,
        [in] bodys.PartLinks,
        [out] voxes.CombineList,
        [out] voxes.CombinePositions,
        [out] voxes.CombineVox,
        [none] players.PlayerCharacter
    );
}
