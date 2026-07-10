/*zox_sys2(DeviceModeDirtySystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(DeviceModeDirty);
    zox_sys_out(DeviceMode);
    zox_sys_out(LastDeviceMode);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(DeviceModeDirty, dirty);
        zox_sys_o(DeviceMode, new);
        zox_sys_o(LastDeviceMode, last);
        if (dirty->value == zox_dirty_active) {
            dirty->value = 0;
            continue;
        }
        if (dirty->value != zox_dirty_trigger) {
            continue;
        }
        // byte old_mode = mode->value;
        last->value = mode->value;
        // mode->value = new_mode->value;
        disable_inputs_until_release(world, e, mode->value);
        dirty->value = zox_dirty_active;
        // zox_log("Devicemode dirty [%i]", mode->value);
    }
} zox_sys_end(DeviceModeDirtySystem);
*/
