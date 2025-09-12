// a block that is for a block vox
entity spawn_prefab_block_vox_meta(
    ecs *world,
    const entity prefab
) {
    zox_prefab_child(prefab);
    zox_prefab_name("block_vox_meta");
    zox_prefab_add(e, ModelLink);
    return e;
}