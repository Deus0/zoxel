// the 3D "Loading" label eases to ahead of its player's camera (huds3_follow), so it stays in view as the head turns
void huds3_follow_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        huds3_follow(world, e, zox_delta_time);
    }
    zox_sys_on_end();
} zoxd_system(huds3_follow_system);

// the 3D view fades to black loading and back after (huds3_dark): rendering's render_darkness, XR's eyes
void huds3_dark_system(iter* it) {
    zox_sys_on_begin();
    const float delta = (float) zox_delta_time;
    if (huds3_dark) {
        huds3_dark_held = 0;
        huds3_darkness = float_min(1, huds3_darkness + delta / huds3_dark_fade_in);
    } else if (huds3_darkness > 0) {
        huds3_dark_held += delta;
        if (huds3_dark_held >= huds3_dark_hold) {
            huds3_darkness = float_max(0, huds3_darkness - delta / huds3_dark_fade_out);
        }
    }
    render_darkness = zox_huds3D ? huds3_darkness : 0;
    zox_sys_on_end();
} zoxd_system(huds3_dark_system);
