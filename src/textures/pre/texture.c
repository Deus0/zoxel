entity spawn_prefab_texture(ecs *world, int2 size) {
    zox_prefab();
    zox_prefab_name("texture");
    zox_add(e, Texture);
    zox_prefab_set(e, TextureData, { 0 });
    zox_prefab_set(e, TextureSize, { size });
    zox_prefab_set(e, TextureDirty, { 0 });
    return e;
}

entity spawn_texture(ecs *world, entity prefab, int2 size) {
    zox_instance(prefab);
    zox_set(e, TextureSize, { size });
    return e;
}
