byte reset_device_button(ZeviceButton* button) {
    byte previous_value = button->value;
    if (devices_get_pressed_this_frame(button->value)) {
        devices_set_pressed_this_frame(&button->value, 0);
    } else if (devices_get_released_this_frame(button->value))  {
        devices_set_released_this_frame(&button->value, 0);
    }
    return previous_value != button->value;
}

// reset each zevice in a system, on EcsOnLoad
zox_sys2(ZeviceButtonResetSystem) {
    zox_sys_begin();
    zox_sys_out(ZeviceButton);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(ZeviceButton, zeviceButton);
        reset_device_button(zeviceButton);
    }
} zox_sys_end(ZeviceButtonResetSystem);
