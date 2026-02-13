void toggle_hierarchy(ecs *world, int32_t keycode) {

    // our logic stuff
    if (keycode != SDLK_y) {
        return;
    }

    entity player = dbg_player;
    zox_geter(player, CanvasLink, canvasLink)
    toggle_ui(world, canvasLink->value, &hierarchy, &spawn_editor_hierarchy);
    spawn_sound_from_file_index(world, prefab_sound, 0);
}
