entity spawn_prefab_block(ecs *world) {
    zox_prefab();
    zox_prefab_name("block");
    zox_add(e, Voxel);
    zox_add(e, Block);
    zox_setv(e, ZoxName, 0);
    zox_setv(e, BlockIndex, 0);
    zox_setv(e, Color, color_white);
    zox_setv(e, BlockModel, zox_block_solid);
    // zox_prefab_set(e, TextureLinks, { 0 });
    zox_setv(e, BlockCollider, zox_block_solid);
    zox_setv(e, BlockSound, 0);
    return e;
}
