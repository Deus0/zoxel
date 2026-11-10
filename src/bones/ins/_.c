// Spawns a skeleton under its owner, linked with SkeletonLink
entity spawn_skeleton(
    ecs* world,
    entity owner)
{
    entity e = zox_ins(world, prefab_skeleton);
    zox_set_unique_name(e, "skeleton");
    if (zox_has(owner, Position3D)) {
        zox_setv(e, Position3D, zox_getv(owner, Position3D));
    }
    if (zox_has(owner, Rotation3D)) {
        zox_setv(e, Rotation3D, zox_getv(owner, Rotation3D));
    }
    zox_set_parent(world, e, owner);
    zox_link(world, owner, SkeletonLink, e);
    return e;
}

// Returns the skeleton of an entity, itself if a skeleton, or its SkeletonLink
entity get_skeleton(
    ecs* world,
    entity e)
{
    if (!zox_valid(e)) {
        return 0;
    }
    if (zox_has(e, Skeleton)) {
        return e;
    }
    entity skeleton = zox_get_link(world, e, SkeletonLink);
    return zox_valid(skeleton) ? skeleton : 0;
}

// The vox a skeletons meshes build from, its owners VoxLink
entity get_skeleton_vox(
    ecs* world,
    entity skeleton)
{
    entity owner = zox_valid(skeleton) ? zox_get_parent(world, skeleton) : 0;
    if (!zox_valid(owner)) {
        return 0;
    }
    return zox_get_link(world, owner, VoxLink);
}
