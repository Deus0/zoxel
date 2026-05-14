entity spawn_prefab_touchscreen(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("touchscreen");
    zox_add_tag(e, Touchscreen);
    zox_prefab_add(e, ScreenDimensions);
    zox_prefab_set(e, DeviceLayout, { 0 });
    return e;
}

entity spawn_touchscreen(ecs *world, entity prefab) {
    zox_instance(prefab);
    zox_name("touchscreen");
    for (byte i = 0; i < fingers_count; i++) {
        entity finger = spawn_zevice_pointer(world, e, i, i);
        zox_add_tag(finger, Finger);
        zox_set_parent(world, finger, e);
        entity e3 = spawn_zevice_stick(world, e, i, i);
        zox_set_parent(world, e3, e);
        zox_set(finger, VirtualZeviceLink, { e3 });
        if (i == 0) {
            zox_set(e3, DeviceButtonType, { zox_device_stick_left });
        } else {
            zox_set(e3, DeviceButtonType, { zox_device_stick_right });
        }
    }
    return e;
}
