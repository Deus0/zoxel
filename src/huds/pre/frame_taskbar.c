entity spawn_prefab_frame_taskbar(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    prefab_add_active_state(world, e, default_outline_color_frame, color_purple);
    return e;
}
