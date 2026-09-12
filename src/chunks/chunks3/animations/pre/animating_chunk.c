entity spawn_prefab_animating_chunk(ecs *world, entity p) {
    const int3 chunk_size = int3_single(16);
    zox_instance(p);
    zox_make_prefab(e);
    zox_prefab_set(e, ChunkSize, { chunk_size });
    zox_prefab_set(e, AnimateChunk, { 1.0 });
    // testing: set colors
    ColorRGBs colors = (ColorRGBs) { 0 };
    initialize_ColorRGBs(&colors, 8);
    for (int i = 0; i < 8; i++) {
        colors.value[i] = (color_rgb) {
            155 - (rand() % 60),
            225 - (rand() % 60),
            255 - (rand() % 60)
        };
    }
    zox_set_ptr(e, ColorRGBs, colors);
    zox_prefab_set(e, ColorsGPULink, { 0 });
    return e;
}
