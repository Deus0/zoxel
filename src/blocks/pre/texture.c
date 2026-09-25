entity spawn_prefab_vox_texture(
    ecs *world,
    entity prefab,
    byte depth)
{
    zox_prefab_child(prefab);
    zox_prefab_name("vox_texture");
    zox_add(e, VoxTexture);
    zox_prefab_set(e, VoxBakeSide, { 0 });
    zox_prefab_set(e, ModelLink, { 0 });
    zox_prefab_set(e, GenerateTexture, { 0 });
    zox_prefab_set(e, Busy, { 1 });
    byte vlength = powers_of_two[depth];
    zox_set(e, TextureSize, { int2_single(vlength) });
    return e;
}
