// an observer (the ui event: a non fragmenting tag, its cleanup observer removes it)
void click_sound_event(iter* it) {
    zox_sys_world();
    zox_sys_begin();
    for (int i = 0; i < it->count; i++) {
        spawn_sound_effect(world, zox_rand() % 100 >= 94 ? sound_effect_click_rare : sound_effect_click, 0);
    }
}
