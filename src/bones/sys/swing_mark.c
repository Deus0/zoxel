// A swing's marks (its events, dat/swings.c): as a swing passes one, SwingMark is set to it (1 its first, the hit), so
//  observers react to the animation: a punch's damage (skills/eve/melee_hit.c), eating's bites (munch, crumbs)
void swing_mark_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SwingStart);
    zox_sys_in(SwingSpeed);
    zox_sys_in(SwingKind);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SwingStart, start);
        zox_sys_i(SwingSpeed, speed);
        zox_sys_i(SwingKind, kind);
        if (start->value <= 0 || speed->value <= 0 || kind->value >= zox_swing_kinds) {
            continue;
        }
        const float now = (float) ((zox_current_time - start->value) / speed->value);
        const float last = now - zox_delta_time / speed->value;
        if (now < 0 || last > 1) {
            continue;
        }
        const swing_data* swing = &swings[kind->value];
        for (byte j = 0; j < swing->marks_count; j++) {
            if (last < swing->marks[j] && now >= swing->marks[j]) {
                zox_set(e, SwingMark, { j + 1 });
            }
        }
    }
    zox_sys_on_end();
} zoxd_system(swing_mark_system);
