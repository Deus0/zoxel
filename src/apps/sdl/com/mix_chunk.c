zoxc(SDLMixChunk, Mix_Chunk*);

ECS_DTOR(SDLMixChunk, ptr, {
    if (ptr->value) {
        free(ptr->value);
    }
})

void on_destroyed_SDLMixChunk(iter *it) {
    byte dbg_log = 0;
    zox_sys_begin();
    zox_sys_out(SDLMixChunk);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(SDLMixChunk, component);
        if (!component->value) {
            continue;
        }
        if (dbg_log) {
            zox_log("SDLMixChunk Destroy [%i]", component->value);
        }
        free(component->value);
        component->value = 0;
    }
}
