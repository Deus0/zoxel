entity spawn_prefab_vox_generated(ecs *world, entity p) {

    zox_prefab_child(p);
    zox_prefab_set(e, Generate, { zox_dirty_trigger });
    zox_prefab_set(e, Color, { color_white });
    zox_add_tag(e, VoxMesh);

    return e;
}
