entity spawn_prefab_vox_generated(
    ecs *world,
    const entity prefab
) {
    zox_prefab_child(prefab);
    zox_prefab_set(e, GenerateVox, { zox_dirty_trigger });
    zox_prefab_set(e, Color, { color_white });
    zox_add_tag(e, VoxMesh);
    return e;
}