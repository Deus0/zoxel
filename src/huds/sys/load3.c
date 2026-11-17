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

// the eyes close loading and open after (huds3_dark, held shut by huds3_dark_hold), the label fading in once shut
void huds3_dark_system(iter* it) {
    zox_sys_on_begin();
    const float delta = (float) zox_delta_time;
    if (huds3_dark || huds3_dark_hold) {
        huds3_dark_held = 0;
        huds3_darkness = float_min(1, huds3_darkness + delta / huds3_dark_close);
    } else if (huds3_darkness > 0) {
        huds3_dark_held += delta;
        if (huds3_dark_held >= huds3_dark_hold_time) {
            huds3_darkness = float_max(0, huds3_darkness - delta / huds3_dark_open);
        }
    }
    if (huds3_dark && huds3_darkness >= 1) {
        huds3_label_fade = float_min(1, huds3_label_fade + delta / huds3_label_fade_time);
    } else if (!huds3_dark) {
        huds3_label_fade = 0;
    }
    zox_sys_on_end();
} zoxd_system(huds3_dark_system);
