// plays triggered sounds through the mixer, a sound with a SoundPosition plays in 3D
void sound_play_system(iter* it) {
    zox_sys_on_begin();
    if (!audio_enabled || nosounds || zox_current_time <= 1) {
        return;
    }
    zox_sys_begin();
    zox_sys_in(TriggerSound);
    zox_sys_in(SoundData);
    zox_sys_in(SoundPosition);  // optional
    zox_sys_in(SoundDelay);     // optional
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(TriggerSound, trigger);
        zox_sys_i(SoundData, data);
        if (trigger->value != zox_dirty_active) {
            continue;
        }
        if (!data->value) {
            zox_loge("Invalid SoundData, triggered.");
            continue;
        }
        zox_mixer_play_delayed(data->value, data->length, SoundPosition_ ? &SoundPosition_[i].value : NULL,
            SoundDelay_ ? SoundDelay_[i].value : 0);
    }
    zox_sys_on_end();
} zoxd_system(sound_play_system);

void sound_play_ref_system(iter* it) {
    zox_sys_on_begin();
    if (!audio_enabled || nosounds || zox_current_time <= 1) {
        return;
    }
    zox_sys_begin();
    zox_sys_in(TriggerSound);
    zox_sys_in(SoundDataRef);
    zox_sys_in(SoundPosition);  // optional
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(TriggerSound, trigger);
        zox_sys_i(SoundDataRef, sound);
        if (trigger->value != zox_dirty_active || !sound->value) {
            continue;
        }
        zox_mixer_play(sound->value, sound->length, SoundPosition_ ? &SoundPosition_[i].value : NULL);
    }
    zox_sys_on_end();
} zoxd_system(sound_play_ref_system);

// the listener (the game camera) moved or turned, 3D sounds follow it
void sound_listener_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_begin();
    zox_sys_in(Position3D);
    zox_sys_in(Rotation3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Position3D, position);
        zox_sys_i(Rotation3D, rotation);
        zox_mixer_set_listener(position->value, rotation->value);
        break;  // one listener
    }
    zox_sys_on_end();
} zoxd_system(sound_listener_system);
