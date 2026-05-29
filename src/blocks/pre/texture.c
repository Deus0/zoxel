entity spawn_prefab_vox_texture(ecs *world, entity p, byte depth) {
    zox_prefab_child(p);
    zox_prefab_name("vox_texture");
    zox_add_tag(e, VoxTexture);
    zox_prefab_set(e, VoxBakeSide, { 0 });
    zox_prefab_set(e, GenerateTexture, { zox_dirty_none });
    byte vlength = powers_of_two[depth];
    zox_set(e, TextureSize, { int2_single(vlength) });
    // zox_prefab_set(e, VoxLink, { 0 });
    return e;
}
