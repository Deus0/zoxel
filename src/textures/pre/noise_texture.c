void add_noise_texture(ecs *world, entity e) {
    zox_add_tag(e, NoiseTexture)
}

void add_animated_noise_texture(ecs *world, entity e) {
    zox_add_tag(e, NoiseTexture)
    zox_prefab_set(e, AnimateTexture, { 0 })
}

entity spawn_prefab_texture_noise(ecs *world, int2 texture_size) {
    zox_prefab();
    zox_prefab_name("texture");
    zox_add_tag(e, Texture);
    zox_prefab_set(e, TextureData, { 0, NULL });
    zox_prefab_set(e, TextureSize, { texture_size });
    zox_prefab_set(e, TextureDirty, { 0 });
    zox_prefab_set(e, Seed, { 666 });
    zox_prefab_set(e, GenerateTexture, { zox_generate_texture_run });
    add_noise_texture(world, e);
    zox_prefab_set(e, FillColor, { color_red })
    return e;
}
