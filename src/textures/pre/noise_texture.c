void add_noise_texture(ecs *world, entity e) {
    zox_add(e, NoiseTexture);
}

void add_animated_noise_texture(ecs *world, entity e) {
    zox_add(e, NoiseTexture);
    zox_set(e, AnimateTexture, { 0 });
}

entity spawn_prefab_texture_noise(ecs *world, int2 texture_size) {
    zox_prefab();
    zox_prefab_name("texture");
    zox_add(e, Texture);
    zox_set(e, TextureData, { 0, NULL });
    zox_set(e, TextureSize, { texture_size });
    zox_set(e, Seed, { 666 });
    zox_set(e, GenerateTexture, { zox_generate_texture_run });
    add_noise_texture(world, e);
    zox_set(e, FillColor, { color_red });
    return e;
}

// an animated water texture: frames of ripples over its FillColor (sys/water.c)
entity spawn_prefab_texture_water(ecs *world, int2 texture_size) {
    zox_prefab();
    zox_prefab_name("texture_water");
    zox_add(e, Texture);
    zox_add(e, WaterTexture);
    zox_setv(e, TextureData, 0);
    zox_setv(e, TextureFrames, 0);
    zox_setv(e, TextureFrameRate, zox_water_frame_rate);
    zox_setv(e, TextureSize, texture_size);
    zox_setv(e, Seed, 666);
    zox_setv(e, GenerateTexture, zox_generate_texture_run);
    zox_setv(e, FillColor, ((color) { 40, 110, 200, zox_water_alpha }));
    return e;
}
