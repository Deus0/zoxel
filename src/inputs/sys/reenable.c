void zevice_button_enable_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ZeviceButton);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ZeviceButton, button);
        const byte has_input = devices_get_pressed(button->value);
        if (!has_input) {
            zox_remove(e, Disabled);
        }
    }
    zox_sys_on_end();
} zoxd_system(zevice_button_enable_system);

void zevice_pointer_enable_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ZevicePointer);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ZevicePointer, zevicePointer);
        const byte has_input = devices_get_pressed(zevicePointer->value);
        if (!has_input) {
            zox_remove(e, Disabled);
        }
    }
    zox_sys_on_end();
} zoxd_system(zevice_pointer_enable_system);

void zevice_stick_enable_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ZeviceStick);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ZeviceStick, zeviceStick);
        byte has_input = zevice_stick_has_input(zeviceStick, joystick_min_cutoff);
        if (!has_input) {
            zox_remove(e, Disabled);
        }
    }
    zox_sys_on_end();
} zoxd_system(zevice_stick_enable_system);
