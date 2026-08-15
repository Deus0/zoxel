entity spawn_prefab_block(ecs *world) {
    zox_prefab();
    zox_prefab_name("block");
    zox_add(e, Voxel);
    zox_add(e, Block);
    zox_prefab_set(e, ZoxName, { 0 });
    zox_prefab_set(e, BlockIndex, { 0 });
    zox_prefab_set(e, Color, { color_white });
    zox_prefab_set(e, BlockModel, { zox_block_solid });
    zox_prefab_set(e, TextureLinks, { 0 });
    zox_prefab_set(e, BlockCollider, { zox_block_solid });
    zox_prefab_set(e, BlockSound, { 0 });
    return e;
}
