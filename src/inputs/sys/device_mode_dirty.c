zox_sys2(DeviceModeDirtySystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(DeviceMode);
    zox_sys_out(DeviceModeDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(DeviceModeDirty, new_mode);
        zox_sys_o(DeviceMode, mode);
        if (!new_mode->value) {
            continue;
        }
        byte old_mode = mode->value;
        mode->value = new_mode->value;
        disable_inputs_until_release(world, e, mode->value, old_mode);
        new_mode->value = 0;
        // zox_log("Devicemode dirty [%i]", mode->value);
    }
} zox_sys_end(DeviceModeDirtySystem);
