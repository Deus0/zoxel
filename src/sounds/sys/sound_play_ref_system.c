zox_sys2(SoundPlayRefSystem) {
    int channel = get_channels_count();
    if (!audio_enabled || channel < 0 || nosounds) {
        return;
    }
    if (zox_current_time <= 1) {
        // zox_log("waiting: %f", zox_current_time);
        return;
    }
    zox_sys_begin();
    zox_sys_in(TriggerSound);
    zox_sys_in(SoundDataRef);
    zox_sys_out(SDLMixChunk);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(TriggerSound, triggerSound);
        zox_sys_i(SoundDataRef, soundDataRef);
        zox_sys_o(SDLMixChunk, chunk);

        if (triggerSound->value == zox_dirty_active) {
            if (soundDataRef->value) {
                // zox_log("+ playing sound ref")
                sdl_play_sound(
                    chunk,
                    soundDataRef->value,
                    soundDataRef->length,
                    mixer_audio_volume,
                    channel);
            }
        }
    }
} zox_sys_end(SoundPlayRefSystem);