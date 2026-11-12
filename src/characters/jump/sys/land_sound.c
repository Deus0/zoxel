// TODO: Make it louder based on velocity of land
void LandSoundSystem(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LandState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(LandState, state);
        if (state->value == land_state_active) {
            spawn_sound_effect(world, sound_effect_land, e);
        }
    }
    zox_sys_on_end();
} zoxd_system(LandSoundSystem);
