// the load ui's second line: the near chunks loaded of those it waits for (stream_end_system's counts), Chunks [3/204]
void load_ui_chunks_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    char text[32];
    snprintf(text, sizeof(text), "Chunks [%u/%u]", zox_stream_end_loaded, zox_stream_end_near);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        set_entity_text(world, e, text);
    }
    zox_sys_on_end();
} zoxd_system(load_ui_chunks_system);
