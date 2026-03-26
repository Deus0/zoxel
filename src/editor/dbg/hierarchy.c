void toggle_hierarchy(ecs *world, int32_t keycode) {

    // our logic stuff
    if (keycode != SDLK_y) {
        return;
    }

    entity player = dbg_player;

    if (!zox_valid(player)) {
        return;
    }

    zox_geter_value(player, CanvasLink, entity, canvas);

    entity e = get_canvas_window(world, canvas, zox_window_hierarchy);

    if (e) {
        zox_log("Deleting Editor Hierarchy");
        zox_delete(e);
        spawn_sound_from_file_index(world, prefab_sound, 1);
    } else {
        entity target = local_realm ? local_realm : canvas;

        if (!zox_valid(target)) {
            zox_log_error("Hierarchy Target is invalid");
            return;
        }

        zox_log("Spawning Editor Hierarchy -> [%s]", zox_get_name(target));

        spawn_window_hierarchy(world, canvas, player, target);
        spawn_sound_from_file_index(world, prefab_sound, 0);
    }
}
