entity spawn_vox_generated(
    ecs *world,
    entity prefab,
    color vox_color)
{
    zox_instance(prefab);
    zox_setv(e, Color, vox_color);
    initialize_voxel_lock(world, e);
    initialize_sides_lock(world, e);
    return e;
}

entity spawn_vox_generated_invisible(
    ecs *world,
    entity prefab,
    color vox_color)
{
    zox_instance(prefab);
    zox_setv(e, Color, vox_color);
    zox_setv(e, RenderDisabled, 1);
    initialize_voxel_lock(world, e);
    initialize_sides_lock(world, e);
    return e;
}
