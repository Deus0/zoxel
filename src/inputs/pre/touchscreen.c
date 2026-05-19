entity spawn_prefab_touchscreen(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("touchscreen");
    zox_add_tag(e, Touchscreen);
    zox_prefab_add(e, ScreenDimensions);
    zox_prefab_set(e, DeviceLayout, { 0 });
    return e;
}

entity spawn_touchscreen_joystick(ecs* world, entity device, entity pointer, byte index, byte type) {
    entity e = spawn_zevice_stick(world, device, index, index);
    zox_set_parent(world, e, device);
    zox_set(e, DeviceButtonType, { type });
    zox_set(pointer, VirtualZeviceLink, { e });
    return e;
}

entity spawn_touchscreen(ecs *world, entity prefab) {
    zox_instance(prefab);
    zox_name("touchscreen");
    // we should spawn virtual joysticks first then link to the fingers
    // then we use the virtual joystick based on the finger spawn
    for (byte i = 0; i < fingers_count; i++) {
        entity e2 = spawn_zevice_pointer(world, e, i, i);
        zox_add_tag(e2, Finger);
        zox_set_parent(world, e2, e);
        byte type = (i == 0) ? zox_device_stick_left : zox_device_stick_right;
        spawn_touchscreen_joystick(world, e, e2, i, type);
        /*entity e3 = spawn_zevice_stick(world, e, i, i);
        zox_set_parent(world, e3, e);
        zox_set(finger, VirtualZeviceLink, { e3 });*/
    }
    return e;
}
