// NOTE: The character3 version with a skeleton
entity spawn_prefab_character3_skeleton(
    ecs *world,
    entity prefab)
{
    entity e = spawn_prefab_character3(
        world,
        prefab,
        zox_character_type_skeleton);
    zox_make_prefab(e);
    zox_prefab_name("character3_skeleton");
    zox_add(e, Skeleton);
    zox_add(e, SkeletonMesh);
    zox_setv(e, SkeletonDirty, 0);
    zox_setv(e, BoneLinks, 0);
    zox_setv(e, BoneIndexes, 0);
    zox_setv(e, BoneIndexGPULink, 0);
    zox_setv(e, RaiseShoulder, 0);
    zox_setv(e, WalkState, 0);
    zox_setv(e, SwingStart, 0);
    zox_setv(e, SwingSpeed, 0);
    // zox_add(e, CharacterUnique);
    // TODO: Add this to prefab in settings
    if (is_paint_skeletons) {
        zox_add(e, PaintedSkeleton);
    }
    return e;
}
