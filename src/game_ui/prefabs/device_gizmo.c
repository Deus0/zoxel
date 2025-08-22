// grid like ui
const float device_gizmo_fade_time = 3;

entity spawn_prefab_device_gizmo(ecs *world, const entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_set(e, DestroyInTime, { device_gizmo_fade_time });
    zox_prefab_add_animation_event(world, e, device_gizmo_fade_time);
    return e;
}

// when switch, spawn, destroy after x seconds, add a cooldown on switching devices
entity spawn_device_gizmo(
    ecs* world,
    const entity canvas,
    const byte device_type
) {
    const byte size = 64;
    const byte position_offset = 16 + size / 2;
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
            .position = (int2) { position_offset, position_offset },
            .anchor = float2_zero,
            .layer = 1,
        },
        .texture_size = int2_single(size),
    };
    const entity e = spawn_icon(world, &spawnIcon);
    zox_set_unique_name(e, "device_gizmo");
    if (device_type == zox_device_mode_gamepad) {
        clone_texture_to_entity(world, e, "device_gamepad");
    } else if (device_type == zox_device_mode_keyboardmouse) {
        clone_texture_to_entity(world, e, "device_keyboard");
    } else if (device_type == zox_device_mode_touchscreen) {
        clone_texture_to_entity(world, e, "device_touchscreen");
    } else {
        clone_texture_to_entity(world, e, "device_none");
    }
    zox_set(e, AnimationStart, { zox_current_time })
    return e;
}
