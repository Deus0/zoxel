// dialogue blips as letters appear, a longer one when the text ends (sound_effect_talk in sounds/fun/effects.c)
zox_sys2(DialogueSoundSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GlyphSpawnedDirty);
    zox_sys_in(AnimateTextEnded);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(GlyphSpawnedDirty, state);
        zox_sys_i(AnimateTextEnded, state2);
        if (state->value != zox_dirty_active && state2->value != zox_dirty_active) {
            continue;
        }
        const SoundEffectType type =
            state2->value == zox_dirty_active ? sound_effect_talk_end :
            rand() % 100 >= 94 ? sound_effect_talk_rare : sound_effect_talk;
        spawn_sound_effect(world, type, 0);
    }
} zox_sys_end(DialogueSoundSystem);
