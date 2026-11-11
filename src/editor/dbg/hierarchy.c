// y: toggles the hierarchy, of the realm's tree (or the canvas's, in the menus)
void toggle_hierarchy(ecs *world, int32_t keycode) {
    if (keycode != zox_key_y) {
        return;
    }
    const entity canvas = get_editor_canvas(world);
    if (!canvas) {
        return;
    }
    const entity hierarchy = zox_get_child_by_id(world, canvas, zox_id(HierarchyUI));
    if (zox_valid(hierarchy)) {
        zox_delete(hierarchy);
        play_editor_toggle_sound(world, 0);
        return;
    }
    const entity root = zox_get_parent_root(world, zox_valid(local_realm) ? local_realm : canvas);
    zox_logv("Spawning Editor Hierarchy -> [%s]", zox_getn(root));
    spawn_window_hierarchy(world, canvas, dbg_player, root);
    play_editor_toggle_sound(world, 1);
}
