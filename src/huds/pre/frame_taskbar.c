entity spawn_prefab_frame_taskbar(ecs *world, entity prefab) {

    zox_prefab_child(prefab);

    zox_prefab_set(e, OutlineColor, { default_outline_color_frame });
    prefab_add_active_state(world, e, color_purple);

    return e;
}
