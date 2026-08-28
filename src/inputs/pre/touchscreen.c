entity spawn_prefab_touchscreen(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("touchscreen");
    zox_add(e, Touchscreen);
    zox_add(e, PixelSize);
    zox_add(e, DeviceLayout);
    return e;
}

entity spawn_prefab_finger(ecs* world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("finger");
    zox_add(e, Finger);
    zox_prefab_setv(e, ID, 0);
    zox_prefab_setv(e, ZeviceDisabled, 1);
    zox_prefab_setv(e, DeviceLayout, 0);
    zox_prefab_setv(e, PixelSize, int2_zero);
    // zox_prefab_setv(e2, ElementLink, 0);
    return e;
}