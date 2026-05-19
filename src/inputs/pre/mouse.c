entity spawn_prefab_mouse(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("mouse");
    zox_add_tag(e, Mouse);
    zox_prefab_set(e, MouseLock, { 0 });
    return e;
}

extern byte zox_dbg_touch_with_mouse;
extern entity spawn_touchscreen_joystick(ecs*, entity, entity, byte, byte);

entity spawn_mouse(ecs *world) {
    zox_instance(prefab_mouse);
    zox_name("mouse");
    entity e2 = spawn_zevice_pointer(world, e, 0, 0);
    zox_set(e2, ZevicePointerRight, { 0 });
    zox_set(e2, ZeviceWheel, { int2_zero });
    zox_set_parent(world, e2, e);
    if (zox_dbg_touch_with_mouse) {
        spawn_touchscreen_joystick(world, e, e2, 0, zox_device_stick_left);
    }
    return e;
}
