void menu_start_triggered(ecs *world, entity player, entity canvas) {
    entity start_menu = find_child_with_tag2(world, canvas, zox_id(MenuStart));
    if (!zox_valid(start_menu)) {
        zox_loge("No MenuStart found on canvas");
    } else {
        // zox_log("Deleting [%s]", zox_get_name(start_menu));
        zox_delete(start_menu);
    }

    double volume = (0.6 + 0.4 * (rand() % 101) / 100.0) * get_volume_sfx();
    spawn_sound_generated(world, prefab_sound_generated, instrument_piano, note_frequencies[16], 2.8f, volume);

#ifdef zox_disable_start_main_menu
    return;
#endif

    spawn_main_menu(world, player, game_name);
}

void button_event_menu_start(ecs *world, ClickEventData event) {
    zox_geter_value(event.clicker, CanvasLink, entity, canvas);
    menu_start_triggered(world, event.clicker, canvas);
}
