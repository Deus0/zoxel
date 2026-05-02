// grid like ui
const float device_gizmo_fade_time = 3;

entity spawn_prefab_device_gizmo(ecs *world, entity prefab) {

    zox_prefab_child(prefab);

    zox_prefab_set(e, DestroyInTime, { device_gizmo_fade_time });
    zox_prefab_add_animation_event(world, e, device_gizmo_fade_time);

    return e;
}

// when switch, spawn, destroy after x seconds, add a cooldown on switching devices
entity spawn_device_gizmo(ecs* world, entity canvas, byte device_type) {

    // Sizing
    byte size = 16 * ui_scale;
    int2 position = int2_single(4 * ui_scale + size / 2);

    SpawnIcon spawnIcon = {
        .canvas = {
            .e = canvas
        },
        .parent = {
            .e = canvas
        },
        .element = {
            .prefab = prefab_device_gizmo,
            .size = int2_single(size),
            .position = position,
            .anchor = float2_zero,
            .layer = 1,
        },
        .texture_size = int2_single(size),
    };

    entity e = spawn_icon(world, &spawnIcon).x;
    zox_set_unique_name(e, "device_gizmo");

    if (device_type == zox_device_mode_gamepad) {
        clone_texture_file_to_entity(world, e, "device_gamepad");
    } else if (device_type == zox_device_mode_keyboardmouse) {
        clone_texture_file_to_entity(world, e, "device_keyboard");
    } else if (device_type == zox_device_mode_touchscreen) {
        clone_texture_file_to_entity(world, e, "device_touchscreen");
    } else {
        clone_texture_file_to_entity(world, e, "device_none");
    }

    zox_set(e, AnimationStart, { zox_current_time });

    return e;
}
