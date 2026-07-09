#include "character.c"
#include "combine.c"
#include "bones.c"
#include "tooltip.c"

void define_systems_bodys(ecs* world) {
    zox_system(
        BodyCombineSystem,
        EcsOnUpdate,
        [in] bodys.BodyDirty,
        [out] bodys.BodySize,
        [out] voxes.CombineList,
        [out] voxes.CombinePositions,
        [out] voxes.CombineVox,
        [out] chunks.NodeDepth,
        [out] blocks.BlockScale,
        [none] players.PlayerCharacter
    );
    zox_system(
        SlotItemTooltipSystem,
        EcsOnUpdate,
        [in] interaction.SelectState,
        [in] slots.DataLink,
        [in] slots.SlotLink,
        [none] elements2.Icon
    );
    zox_system_1(
        CharacterBodySpawnSystem,
        zoxp_mainthread,
        [in] characters.GenerateCharacter,
        [in] realms.RealmLink,
        [out] bodys.BodyDirty,
        [none] players.PlayerCharacter
    );
    zox_system_1(
        CharacterBoneSpawnSystem,
        zoxp_mainthread,
        [in] bodys.BodyDirty,
        [in] bodys.BodySize,
        [in] blocks.BlockScale,
        [out] bones.BoneLinks,
        [out] bones.SkeletonDirty,
        [none] bones.Skeleton
    );
}
