#include "character.c"
#include "combine.c"
#include "bones.c"
#include "tooltip.c"

void zox_systems_bodys(ecs* world) {
    zox_system(
        BodyCombineSystem,
        zoxp_update,
        [in] chunks3.VoxLink,
        [out] parts.BodyDirty,
        [out] parts.BodySize,
        [out] blocks.BlockScale,
        [none] bones.SkeletonLink,
    );
    zox_system_1(
        CharacterBodySpawnSystem,
        zoxp_spawn,
        [in] characters.GenerateCharacter,
        [out] parts.BodyDirty,
        [none] bones.SkeletonLink,
    );
    zox_system_1(
        CharacterBoneSpawnSystem,
        zoxp_spawn,
        [in] parts.BodySize,
        [in] blocks.BlockScale,
        [in] bones.SkeletonLink,
        [out] parts.BodyDirty,
    );
    zox_system(
        SlotItemTooltipSystem,
        zoxp_update,
        [in] interactions.SelectState,
        [in] slots.DataLink,
        [none] slots.SlotUser,
        [none] elements2.Icon,
    );
}
