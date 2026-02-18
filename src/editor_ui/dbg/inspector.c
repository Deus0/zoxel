void toggle_inspector(ecs *world, int32_t keycode) {

    // our logic stuff
    if (keycode != SDLK_u) {
        return;
    }

    entity player = dbg_player;
    if (!zox_valid(player)) {
        return;
    }

    zox_geter(player, CanvasLink, canvasLink);
    toggle_ui(world, canvasLink->value, &inspector, &spawn_inspector);
    spawn_sound_from_file_index(world, prefab_sound, 0);
}
