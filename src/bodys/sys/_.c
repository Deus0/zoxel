#include "character.c"
#include "combine.c"
#include "bones.c"
#include "tooltip.c"

void define_systems_bodys(ecs* world) {
    zox_system(
        BodyCombineSystem,
        zoxp_update,
        [out] bodys.BodyDirty,
        [out] bodys.BodySize,
        [out] voxes.CombineList,
        [out] voxes.CombinePositions,
        [out] voxes.CombineVox,
        [out] chunks.NodeDepth,
        [out] blocks.BlockScale,
        [none] bones.Skeleton
        // [none] players.PlayerCharacter
    );
    zox_system(
        SlotItemTooltipSystem,
        zoxp_update,
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
        [none] bones.Skeleton
    );
    zox_system_1(
        CharacterBoneSpawnSystem,
        zoxp_mainthread,
        [in] bodys.BodySize,
        [in] blocks.BlockScale,
        [out] bodys.BodyDirty,
        [out] bones.BoneLinks,
        [out] bones.SkeletonDirty,
        [none] bones.Skeleton
    );
}
