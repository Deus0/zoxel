entity spawn_prefab_tilemap(ecs *world) {
    zox_prefab();
    zox_prefab_name("tilemap");
    zox_add(e, Tilemap);
    zox_add(e, Texture);
    zox_setv(e, TilemapSize, int2_one);
    zox_setv(e, TextureData, 0);
    zox_setv(e, TextureSize, int2_zero);
    zox_setv(e, TextureGPULink, 0);
    zox_setv(e, MaterialGPULink, 0);
    zox_setv(e, TextureLinks, 0);
    zox_setv(e, TilemapUVs, 0);
    zox_setv(e, GenerateTexture, 0);
    zox_add(e, PreInitialize);
    return e;
}

entity spawn_tilemap(
    ecs *world,
    entity prefab,
    entity realm)
{
    entity shader = shader_textured3D;
    zox_instance(prefab);
    zox_name("tilemap");
    zox_set_parent(world, e, realm);
    zox_link(world, e, RealmLink, realm);
    if (!shader) {
        return e;
    }
    zox_link(world, e, ShaderLink, shader);
    guint2 gpu_shader = zox_getv(shader, ShaderGPULink);
    guint gpu_material = spawn_gpu_material_program(gpu_shader);
    if (gpu_material) {
        MaterialTextured3D attributes = create_MaterialTextured3D(gpu_material);
        zox_setv(e, MaterialGPULink, gpu_material);
        zox_set_data(e, MaterialTextured3D, attributes);
    }
    return e;
}
