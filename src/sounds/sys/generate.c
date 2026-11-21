// todo: alter frequency over time during sound
void sound_generate_system(iter* it) {
    zox_sys_on_begin();
    if (nosounds || !master_volume) {
        return;
    }
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(InstrumentType);
    zox_sys_in(SoundLength);
    zox_sys_in(SoundFrequency);
    zox_sys_in(SoundVolume);
    zox_sys_in(SoundData);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SoundLength, soundLength);
        zox_sys_i(SoundFrequency, soundFrequency);
        zox_sys_i(InstrumentType, instrumentType);
        zox_sys_i(SoundVolume, soundVolume);
        zox_sys_i(SoundData, soundData);
        zox_remove(e, GenerateSound);
        if (!soundData->value) {
            zox_logw("Sound Data missing in [sound_generate_system]");
            continue;
        }
        float noise = sound_noise * (zox_rand() % 101) / 100.0f;
        if (!generate_instrument_samples(
            instrumentType->value,
            soundFrequency->value,
            soundLength->value,
            soundVolume->value,
            noise,
            soundData->value,
            soundData->length))
        {
            zox_logw("Unknown instrument [%i] in [sound_generate_system]", instrumentType->value);
        }
        zox_add(e, Dirty);
        zox_logv("+ generated sound (%f)", soundVolume->value);
        zox_sys_increment();
    }
    zox_sys_on_end();
} zoxd_system(sound_generate_system);
