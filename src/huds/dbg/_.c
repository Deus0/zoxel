void toggle_fps_viewer(ecs *world, int32_t keycode) {
    if (keycode != SDLK_z) {
        return;
    }

    entity player = dbg_player;
    if (!zox_valid(player)) {
        return;
    }

    zox_geter_value(player, CanvasLink, entity, canvas);
    zox_geter(player, ElementLinks, elements);
    entity ui = player_toggle_ui_id(
        world,
        canvas,
        elements,
        zox_id(FPSDisplay),
        spawn_fps_display);
    if (ui) {
        zox_muter(player, ElementLinks, elements2);
        add_to_ElementLinks(elements2, ui);
        zox_set(ui, ElementHolder, { player });
    }
    // toggle_ui_with_tag(spawn_fps_display, FPSDisplay);
    zox_log("FPSViewer %s", ui ? "Enabled" : "Disabled");
}
