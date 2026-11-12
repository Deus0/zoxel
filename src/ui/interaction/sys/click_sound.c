void ClickSoundSystem(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    for (int i = 0; i < it->count; i++) {
        spawn_sound_effect(world, rand() % 100 >= 94 ? sound_effect_click_rare : sound_effect_click, 0);
    }
    zox_sys_on_end();
} zoxd_system(ClickSoundSystem);
