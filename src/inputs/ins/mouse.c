extern byte zox_dbg_touch_with_mouse;
extern entity spawn_touchscreen_joystick(ecs*, entity, entity, byte, byte);

entity spawn_mouse(ecs *world) {
    zox_instance(prefab_mouse);
    zox_name("mouse");
    entity e2 = spawn_zevice_pointer(world, prefab_mouse_pointer, e, 0, 0);
    zox_set_unique_name(e2, "mouse_pointer");
    zox_set_parent(world, e2, e);
    if (zox_dbg_touch_with_mouse) {
        spawn_touchscreen_joystick(world, e, e2, 0, zox_device_stick_left);
    }
    return e;
}
