// NOTE: used for virtual joysticks to see if a target was raycasted
zox_sys2(RaycasterResulterSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DeviceLink);
    zox_sys_in(RaycasterTarget);
    zox_sys_out(RaycasterResult);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(DeviceLink, device);
        zox_sys_i(RaycasterTarget, target);
        zox_sys_o(RaycasterResult, result);
        if (zox_valid(device->value) && !zox_getv(device->value, DeviceDisabled)) {
            result->value = target->value;
        }
    }
} zox_sys_end(RaycasterResulterSystem);
