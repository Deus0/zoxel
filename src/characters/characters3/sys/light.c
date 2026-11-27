// a character lit by the terrain round it, as minivoxes are: the light at its head (its chunk's light octree), its
//  Brightness easing to it, so stepping into shade fades, not pops; the sky's light is the shaders' (view.glsl)
#define character_light_head 0.6f       // metres above its middle: its head's voxel
#define character_light_ease 6.0f       // of the way a second

void character_light_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Position3D);
    zox_sys_out(Brightness);
    const float ease = float_min(1, (float) zox_delta_time * character_light_ease);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Position3D, position);
        zox_sys_o(Brightness, brightness);
        const entity chunk = zox_get_link(world, e, ChunkLink);
        float light = chunk_light_at(world, chunk, float3_add(position->value, (float3) { 0, character_light_head, 0 }));
        if (light < 0) {
            light = chunk_light_at(world, chunk, position->value);     // its head in the chunk above
        }
        if (light < 0) {
            continue;
        }
        brightness->value += (light - brightness->value) * ease;
    }
    zox_sys_on_end();
} zoxd_system(character_light_system);
