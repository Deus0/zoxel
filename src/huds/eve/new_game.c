void button_event_new_game(ecs *world, ClickEventData event) {
    entity player = event.clicker;
    entity canvas = zox_get_link(world, player, CanvasLink);
    if (!zox_valid(canvas)) {
        return;
    }
    entity main_menu = zox_get_child_by_id(world, canvas, zox_id(MenuMain));
    if (zox_valid(main_menu)) {
        close_ui_related_tooltip(world, main_menu);
        zox_delete(main_menu);
    }
    entity game = zox_get_parent(world, player);
    // Spawn a new Realm!
    lint seed = get_unique_time_seed();
    spawn_realm(
        world,
        game,
        seed);
    delay_event(
        world,
        &delay_spawn_menu_new_realm,
        player,
        0.01);
}
