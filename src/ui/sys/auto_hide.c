// Elements that hide (AutoHide): shrink away once idle for its seconds, pop back in when poked (ui_poke), i.e. the
//  player's stat bars, shown while they change; main thread, popping spawns tweens
void auto_hide_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(AutoHide);
    zox_sys_in(AutoHideTime);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(AutoHide, delay);
        zox_sys_i(AutoHideTime, time);
        const byte idle = zox_current_time - time->value > delay->value;
        const byte hidden = zox_has(e, AutoHidden);
        if (idle && !hidden) {
            ui_shrink(world, e);
            zox_add(e, AutoHidden);
        } else if (!idle && hidden) {
            ui_pop_in(world, e);
            zox_remove(e, AutoHidden);
        }
    }
    zox_sys_on_end();
} zoxd_system(auto_hide_system);
