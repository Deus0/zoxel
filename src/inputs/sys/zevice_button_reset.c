// reset each zevice in a system, on EcsOnLoad
zox_sys2(ZeviceButtonResetSystem) {
    byte dbg_log = 0;
    zox_sys_begin();
    zox_sys_out(ZeviceButton);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(ZeviceButton, button);
        if (devices_get_pressed_this_frame(button->value)) {
            devices_set_pressed_this_frame(&button->value, 0);
            if (dbg_log) {
                zox_log("Pressed this frame is resetting [%s]", zox_sys_e_name);
            }
        } else if (devices_get_released_this_frame(button->value))  {
            devices_set_released_this_frame(&button->value, 0);
            if (dbg_log) {
                zox_log("Releasing this frame is resetting [%s]", zox_sys_e_name);
            }
        }
    }
} zox_sys_end(ZeviceButtonResetSystem);
