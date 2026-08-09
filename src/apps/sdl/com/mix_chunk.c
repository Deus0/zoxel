zoxc(SDLMixChunk, Mix_Chunk*);

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
        Mix_FreeChunk(component->value);
        // free(component->value);
        component->value = 0;
    }
}

byte sdl_play_sound(SDLMixChunk* chunk, const float* data, int length, float volume, int channel) {
    if (use_sdl_audio) {
        return sdl_audio_play_sound(data, length, 2);
    }
    if (!length || !volume || !data) {
        return 0;
    }
    if (!chunk->value) {
        chunk->value = malloc(sizeof(Mix_Chunk));
    }
    if (!chunk->value) {
        return 0;
    }
    *chunk->value = (Mix_Chunk) {
        .allocated = 0,
        .volume = (Uint8)(volume * 128.f),
        .alen = length * sizeof(float),
        .abuf = (void*) data,
    };
    if (Mix_PlayChannel(channel, chunk->value, 0) == -1) {
        zox_loge("Failed to play sound: %s", Mix_GetError());
        return 0;
    }
    return 1;
}