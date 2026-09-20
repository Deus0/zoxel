#include "character.c"
#include "combine.c"
#include "bones.c"
#include "tooltip.c"

void zox_systems_bodys(ecs* world) {
    zox_system(
        BodyCombineSystem,
        zoxp_update,
        [out] parts.BodyDirty,
        [out] parts.BodySize,
        [out] chunks3.CombineList,
        [out] chunks3.CombinePositions,
        [out] chunks3.CombineVox,
        [out] chunks.NodeDepth,
        [out] blocks.BlockScale,
        [none] bones.Skeleton
        // [none] players.PlayerCharacter
    );
    zox_system_1(
        CharacterBodySpawnSystem,
        zoxp_spawn,
        [in] characters.GenerateCharacter,
        [out] parts.BodyDirty,
        [none] bones.Skeleton
    );
    zox_system_1(
        CharacterBoneSpawnSystem,
        zoxp_spawn,
        [in] parts.BodySize,
        [in] blocks.BlockScale,
        [out] parts.BodyDirty,
        [out] rendering3.BoneLinks,
        [out] bones.SkeletonDirty,
        [none] bones.Skeleton
    );
    zox_system(
        SlotItemTooltipSystem,
        zoxp_update,
        [in] interaction.SelectState,
        [in] slots.DataLink,
        // [in] slots.SlotLink,
        [none] slots.SlotUser,
        [none] elements2.Icon
    );
}
