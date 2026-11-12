// todo: alter frequency over time during sound
void SoundGenerateSystem(iter* it) {
    zox_sys_on_begin();
    if (nosounds || !master_volume) {
        return;
    }
    zox_sys_begin();
    zox_sys_in(InstrumentType);
    zox_sys_in(SoundLength);
    zox_sys_in(SoundFrequency);
    zox_sys_in(SoundVolume);
    zox_sys_in(GenerateSound);
    zox_sys_in(SoundData);
    zox_sys_out(TriggerSound);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(SoundLength, soundLength);
        zox_sys_i(SoundFrequency, soundFrequency);
        zox_sys_i(InstrumentType, instrumentType);
        zox_sys_i(SoundVolume, soundVolume);
        zox_sys_i(GenerateSound, generateSound);
        zox_sys_i(SoundData, soundData);
        zox_sys_o(TriggerSound, triggerSound);
        if (generateSound->value != zox_dirty_active) {
            continue;
        }
        if (!soundData->value) {
            zox_logw("Sound Data missing in [SoundGenerateSystem]");
            continue;
        }
        float noise = sound_noise * (rand() % 101) / 100.0f;
        if (!generate_instrument_samples(
            instrumentType->value,
            soundFrequency->value,
            soundLength->value,
            soundVolume->value,
            noise,
            soundData->value,
            soundData->length))
        {
            zox_logw("Unknown instrument [%i] in [SoundGenerateSystem]", instrumentType->value);
        }
        triggerSound->value = zox_dirty_trigger;
        zox_logv("+ generated sound (%f)", soundVolume->value);
        zox_sys_increment();
    }
    zox_sys_on_end();
} zoxd_system(SoundGenerateSystem);
