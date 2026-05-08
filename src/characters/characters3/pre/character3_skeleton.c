entity spawn_prefab_character3_skeleton(ecs *world, entity p) {
    entity e = spawn_prefab_character3(world, p, zox_character_type_skeleton);
    zox_make_prefab(e);
    zox_prefab_name("character3_skeleton");
    zox_prefab_set(e, Children, { 0 });
    zox_add_tag(e, Skeleton);
    zox_add_tag(e, SkeletonMesh);
    zox_prefab_set(e, SkeletonDirty, { 0 });
    zox_prefab_set(e, HeadBoneLink, { 0 });
    zox_prefab_set(e, BoneLinks, { 0 });
    zox_prefab_set(e, BoneIndexes, { 0 });
#ifdef zox_paint_skeletons
    zox_add_tag(e, PaintedSkeleton);
#endif

    return e;
}
