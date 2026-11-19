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

// the eyes close loading and open after (load_dark, held shut by load_dark_hold): each player's camera's CameraLids,
//  the 3D label fading in once they're shut
//  - only while loading closes them or opens them after (load_lids), so it leaves sleeping's lids alone
void load_lids_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    const float delta = (float) zox_delta_time;
    const byte closing = load_dark || load_dark_hold;
    if (closing) {
        load_lids = 1;
        load_dark_held = 0;
    } else if (load_lids) {
        load_dark_held += delta;
    }
    byte shut = 1, open = 1;
    for (int i = 0; load_lids && i < players_playing; i++) {
        const entity camera = zox_get_link(world, zox_players[i], CameraLink);
        if (!zox_valid(camera) || !zox_has(camera, CameraLids)) {
            continue;
        }
        float lids = zox_getv(camera, CameraLids);
        if (closing) {
            lids = float_min(1, lids + delta / load_dark_close);
        } else if (load_dark_held >= load_dark_hold_time) {
            lids = float_max(0, lids - delta / load_dark_open);
        }
        zox_setv(camera, CameraLids, lids);
        shut &= lids >= 1;
        open &= lids <= 0;
    }
    if (!closing && open) {
        load_lids = 0;
    }
    if (load_dark && load_lids && shut) {
        huds3_label_fade = float_min(1, huds3_label_fade + delta / huds3_label_fade_time);
    } else if (!load_dark) {
        huds3_label_fade = 0;
    }
    zox_sys_on_end();
} zoxd_system(load_lids_system);
