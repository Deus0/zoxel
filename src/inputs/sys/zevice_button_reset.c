// reset each zevice in a system, on EcsOnLoad
zox_sys2(ZeviceButtonResetSystem) {
    zox_sys_begin();
    zox_sys_out(ZeviceButton);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(ZeviceButton, zeviceButton);
        reset_device_button(zeviceButton);
    }
} zox_sys_end(ZeviceButtonResetSystem);
