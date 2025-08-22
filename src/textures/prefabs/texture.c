entity spawn_prefab_texture(
    ecs *world,
    const int2 texture_size
) {
    zox_prefab();
    zox_prefab_name("texture");
    zox_add_tag(e, Texture);
    zox_prefab_set(e, TextureData, { 0, NULL });
    zox_prefab_set(e, TextureSize, { texture_size });
    zox_prefab_set(e, TextureDirty, { 0 });
    return e;
}

entity spawn_texture(
    ecs *world,
    const entity prefab,
    int2 size
) {
    zox_instance(prefab);
    zox_set(e, TextureSize, { size });
    return e;
}
