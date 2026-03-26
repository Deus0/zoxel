void toggle_inspector(ecs *world, int32_t keycode) {

    // our logic stuff
    if (keycode != SDLK_u) {
        return;
    }

    entity player = dbg_player;

    if (!zox_valid(player)) {
        return;
    }

    zox_geter_value(player, CanvasLink, entity, canvas);

    entity e = get_canvas_window(world, canvas, zox_window_inspector);

    if (e) {
        zox_log("Deleting Editor Inspector");
        zox_delete(e);
        spawn_sound_from_file_index(world, prefab_sound, 1);
    } else {
        zox_log("Spawning Editor Inspector");
        spawn_window_inspector(world, canvas, player, canvas);
        spawn_sound_from_file_index(world, prefab_sound, 0);
    }
}
