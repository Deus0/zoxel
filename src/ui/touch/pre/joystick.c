entity spawn_prefab_virtual_joystick(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("virtual_joystick");
    zox_add_tag(e, JoystickUI);
    zox_prefab_add(e, ZeviceLink);
    // zox_add_tag(e, Button);
    // zox_add_tag(e, FrameTexture);
    // zox_prefab_set(e, FrameCorner, { 2 });
    // zox_prefab_set(e, OutlineThickness, { 2 });
    // zox_prefab_set(e, Color, { virtual_joystick_color });
    return e;
}
