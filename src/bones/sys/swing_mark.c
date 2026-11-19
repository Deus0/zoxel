// A swing's marks (its events, dat/swings.c): as a swing reaches one, SwingMark is set to it (1 its first, the hit), so
//  observers react to the animation: a punch's damage (skills/eve/melee_hit.c), eating's bites (munch, crumbs)
//  - in order, each once: the next is SwingMark + 1 (a swing starting sets it 0), fired once the swing's reached it,
//    one a frame; a long frame doesn't skip one (it was found from the frame's delta, capped: a hitch missed bites
//    and hits, a missed first bite ate nothing)
void swing_mark_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SwingStart);
    zox_sys_in(SwingSpeed);
    zox_sys_in(SwingKind);
    zox_sys_in(SwingMark);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SwingStart, start);
        zox_sys_i(SwingSpeed, speed);
        zox_sys_i(SwingKind, kind);
        zox_sys_i(SwingMark, mark);
        if (start->value <= 0 || speed->value <= 0 || kind->value >= zox_swing_kinds) {
            continue;
        }
        const swing_data* swing = &swings[kind->value];
        if (mark->value >= swing->marks_count) {
            continue;
        }
        const float now = (float) ((zox_current_time - start->value) / speed->value);
        if (now >= swing->marks[mark->value]) {
            zox_set(e, SwingMark, { mark->value + 1 });
        }
    }
    zox_sys_on_end();
} zoxd_system(swing_mark_system);
