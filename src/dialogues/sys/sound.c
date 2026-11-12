// the speaker babbles the dialogue as it types out, a syllable at a time, from where they stand (3D)
zox_sys2(DialogueSoundSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GlyphSpawnedDirty);
    zox_sys_in(TargetText);
    zox_sys_in(TextData);
    zox_sys_out(VoiceIndex);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GlyphSpawnedDirty, state);
        zox_sys_i(TargetText, target);
        zox_sys_i(TextData, data);
        zox_sys_o(VoiceIndex, next);
        if (state->value != zox_dirty_active) {
            continue;
        }
        // every letter shown since the last one said (a slow frame shows a few)
        const int shown = data->length;
        if (next->value > shown) {
            next->value = 0;
        }
        const entity speaker = zox_get_link(world, e, SpeakerLink);
        const Voice voice = get_speaker_voice(world, speaker);
        for (; next->value < shown; next->value++) {
            DialogueSyllable syllable;
            if (dialogue_syllable_at(target->value, next->value, &syllable)) {
                spawn_voice_syllable(world, &voice, syllable.consonant, syllable.vowel, syllable.inflection,
                    1.3f * syllable.volume * get_volume_sfx(), speaker);
            }
        }
    }
} zox_sys_end(DialogueSoundSystem);
