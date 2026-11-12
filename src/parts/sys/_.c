#include "character.c"
#include "combine.c"
#include "bones.c"
#include "tooltip.c"

void zox_systems_bodys(ecs* world) {
    zox_system(
        body_combine_system,
        zoxp_update,
        [out] parts.BodyDirty,
        [out] parts.BodySize,
        [out] blocks.BlockScale,
        [none] bones.SkeletonOwner,
    );
    zox_system_1(
        character_body_spawn_system,
        zoxp_spawn,
        [in] characters.GenerateCharacter,
        [out] parts.BodyDirty,
        [none] bones.SkeletonOwner,
    );
    zox_system_1(
        character_bone_spawn_system,
        zoxp_spawn,
        [in] parts.BodySize,
        [in] blocks.BlockScale,
        [out] parts.BodyDirty,
        [none] bones.SkeletonOwner,
    );
    zox_system(
        slot_item_tooltip_system,
        zoxp_update,
        [in] interactions.SelectState,
        [none] slots.SlotUser,
        [none] elements2.Icon,
    );
}
