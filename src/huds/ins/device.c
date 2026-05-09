// grid like ui
float device_gizmo_fade_time = 3;

// when switch, spawn, destroy after x seconds, add a cooldown on switching devices
entity spawn_device_gizmo(ecs* world, entity canvas, byte device_type) {
    entity prefab = prefab_element_shell; // prefab_device_gizmo;
    int2 size = int2_single(32 * ui_scale);
    int2 position = int2_single(16 * ui_scale);
    position.x = 32 * ui_scale;   // testing anchor
    float2 position_anchor = float2_zero;
    entity e = spawn_element3(world, prefab, canvas, position_anchor, position, size, size, color_white, color_white);
    zox_name("device_gizmo");
    zox_set(e, Layer2D, { 1 });
    zox_set(e, Alpha, { 1 });
    zox_set(e, AnimationStart, { 0 });
    zox_set(e, FadeOutEvent, { device_gizmo_fade_time });
    zox_set(e, DestroyInTime, { device_gizmo_fade_time });
    if (device_type == zox_device_mode_gamepad) {
        clone_texture_file_to_entity(world, e, "device_gamepad");
    } else if (device_type == zox_device_mode_keyboardmouse) {
        clone_texture_file_to_entity(world, e, "device_keyboard");
    } else if (device_type == zox_device_mode_touchscreen) {
        clone_texture_file_to_entity(world, e, "device_touchscreen");
    } else {
        clone_texture_file_to_entity(world, e, "device_none");
    }
    // zox_log("+ device gizmo %lu - %s: %i", e, zox_get_name(e), device_type);
    return e;
}
