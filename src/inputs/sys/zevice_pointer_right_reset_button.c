zox_sys2(ZevicePointerRightResetSystem) {
    zox_sys_begin();
    zox_sys_out(ZevicePointerRight);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(ZevicePointerRight, click);
        click->value = reset_button_state(click->value);
    }
} zox_sys_end(ZevicePointerRightResetSystem);