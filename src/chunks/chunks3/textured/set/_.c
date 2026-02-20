// NOTE: Turns out the dig method was fastest
// We have 3 modes, old, new, and hybrid
#define zox_chunk3t_mode_dig 0
#define zox_chunk3t_mode_new 1
#define zox_chunk3t_mode_mix 2

byte zox_chunk3t_mode = zox_chunk3t_mode_dig;

// byte zox_chunk3t_split = 0;

void set_chunk_split(ecs* world, void* value) {
    (void) world;
    byte new_value = *(byte*) value;
    zox_split_textured_quads = new_value;
    // zox_log("Set Chunks Split [%i]", new_value);
}

void initialize_settings_chunks3_textured(ecs *world) {
    zoxs_new_byte("chunk split", set_chunk_split, zox_split_textured_quads);
}
