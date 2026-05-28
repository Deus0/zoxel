// NOTE: The character3 version with a skeleton
entity spawn_prefab_character3_skeleton(ecs *world, entity prefab) {
    entity e = spawn_prefab_character3(world, prefab, zox_character_type_skeleton);
    zox_make_prefab(e);
    zox_prefab_name("character3_skeleton");
    zox_add_tag(e, Skeleton);
    zox_add_tag(e, SkeletonMesh);
    zox_prefab_set(e, SkeletonDirty, { 0 });
    zox_prefab_set(e, BoneLinks, { 0 });
    zox_prefab_set(e, BoneIndexes, { 0 });
    zox_prefab_set(e, BoneIndexGPULink, { 0 });
    zox_prefab_set(e, HeadBoneLink, { 0 });
    zox_prefab_set(e, HandBoneLink, { 0 });
    zox_prefab_set(e, ShoulderBoneLink, { 0 });
    zox_prefab_set(e, RaiseShoulder, { 1 });
    return e;
}
