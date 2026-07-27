entity spawn_prefab_tilemap(ecs *world) {
    zox_prefab();
    zox_prefab_name("tilemap");
    zox_add_tag(e, Tilemap);
    zox_add_tag(e, Texture);
    zox_prefab_set(e, Initialize, { 1 });
    zox_prefab_set(e, RealmLink, { 0 });
    zox_prefab_set(e, TilemapSize, { { 1, 1 } });
    zox_prefab_set(e, TextureData, { 0 });
    zox_prefab_set(e, TextureSize, { int2_zero });
    zox_prefab_set(e, TextureDirty, { 0 });
    // zox_prefab_set(e, Seed, { 666 });
    zox_prefab_set(e, GenerateTexture, { 0 });
    zox_prefab_set(e, TextureGPULink, { 0 });
    zox_prefab_set(e, MaterialGPULink, { 0 });
    zox_prefab_set(e, ShaderLink, { 0 });
    zox_prefab_set(e, TextureLinks, { 0 });
    zox_prefab_set(e, TilemapUVs, { 0 });
    return e;
}

entity spawn_tilemap(ecs *world, entity prefab) {
    zox_instance(prefab);
    zox_name("tilemap");
    if (!shader_textured3D) {
        return e;
    }
    guint2 shader = zox_getv(shader_textured3D, ShaderGPULink);
    zox_set(e, ShaderLink, { shader_textured3D });
    guint material = spawn_gpu_material(world, e, shader);
    if (!material) {
        zox_loge("tilemap material failed  to initialize");
        return e;
    }
    MaterialTextured3D attributes = create_MaterialTextured3D(material);
    zox_set_data(e, MaterialTextured3D, attributes);
    return e;
}
