void set_chunk_split(ecs* world, void* value) {
    (void) world;
    byte new_value = *(byte*) value;
    zox_split_textured_quads = new_value;
    // zox_log("Set Chunks Split [%i]", new_value);
}

/*void initialize_settings_chunks3_textured(ecs *world) {
#ifdef zox_debug
    zoxs_new_byte("chunk split", set_chunk_split, zox_split_textured_quads);
#endif
}*/
