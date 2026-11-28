// a character lit by the terrain round it, as minivoxes are: the light at its head (its chunk's light octrees), its
//  Brightness (the sky's) and BlockBrightness (blocks': glow stones, lamps) easing to them, so stepping into shade
//  fades, not pops; the sky's light on the sky's part is the shaders' (view.glsl zox_light)
#define character_light_head 0.6f       // metres above its middle: its head's voxel
#define character_light_ease 6.0f       // of the way a second

// a channel's light at its head, else its middle (its head in the chunk above), under 0 none
static inline float character_light_of(ecs* world, entity chunk, float3 position, byte channel) {
    const float light = chunk_light_at(world, chunk, float3_add(position, (float3) { 0, character_light_head, 0 }), channel);
    return light >= 0 ? light : chunk_light_at(world, chunk, position, channel);
}

void character_light_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Position3D);
    zox_sys_out(Brightness);
    zox_sys_out(BlockBrightness);
    const float ease = float_min(1, (float) zox_delta_time * character_light_ease);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Position3D, position);
        zox_sys_o(Brightness, brightness);
        zox_sys_o(BlockBrightness, block_brightness);
        const entity chunk = zox_get_link(world, e, ChunkLink);
        const float sky = character_light_of(world, chunk, position->value, zox_light_sky);
        if (sky < 0) {
            continue;
        }
        const float block = character_light_of(world, chunk, position->value, zox_light_block);
        brightness->value += (sky - brightness->value) * ease;
        block_brightness->value += ((block < 0 ? 0 : block) - block_brightness->value) * ease;
    }
    zox_sys_on_end();
} zoxd_system(character_light_system);
