void menu_start_triggered(ecs *world, entity player, entity canvas) {

    // find_child_with_tag(canvas, MenuStart, menu);
    entity menu = find_child_with_tag_recursive(world, canvas, zox_id(MenuStart));
    if (!menu) {
        zox_log_error("No MenuStart found on canvas")
        return;
    }

    zox_delete(menu);
    spawn_main_menu(world, player, game_name);
    double volume = (0.6 + 0.4 * (rand() % 101) / 100.0) * get_volume_sfx();
    spawn_sound_generated(world, prefab_sound_generated, instrument_piano, note_frequencies[16], 2.8f, volume);
}

void button_event_menu_start(ecs *world, ClickEventData event) {
    zox_geter_value(event.clicker, CanvasLink, entity, canvas);
    menu_start_triggered(world, event.clicker, canvas);
}
