// grid like ui
float device_gizmo_fade_time = 3;

// when switch, spawn, destroy after x seconds, add a cooldown on switching devices
entity spawn_device_gizmo(ecs* world, entity canvas, byte device_type) {
    entity prefab = prefab_element_shell;
    int2 size = int2_single(32 * ui_scale);
    int2 position = int2_single(16 * ui_scale);
    position.x = 32 * ui_scale;   // testing anchor
    float2 position_anchor = float2_zero;
    byte layer = max_layers2D - 2;
    // spawn it
    entity e = spawn_ui(world, prefab, canvas, position_anchor, position, size, size);
    zox_name("device_gizmo");
    zox_set(e, Layer2D, { layer });
    zox_set(e, Alpha, { 1 });
    zox_set(e, AnimationStart, { zox_current_time });
    zox_set(e, FadeOutTime, { device_gizmo_fade_time });
    zox_set(e, DestroyInTime, { device_gizmo_fade_time });
    char* texture_name;
    if (device_type == zox_device_mode_gamepad) {
        texture_name = "device_gamepad";
    } else if (device_type == zox_device_mode_keyboardmouse) {
        texture_name = "device_keyboard";
    } else if (device_type == zox_device_mode_touchscreen) {
        texture_name = "device_touchscreen";
    } else {
        texture_name = "device_none";
    }
    clone_texture_file_to_entity(world, e, texture_name);
    // zox_log("+ Device UI [%i] on Canvas [%s]; prefab [%s]", device_type, zox_get_name(canvas), zox_get_name(prefab));
    return e;
}
