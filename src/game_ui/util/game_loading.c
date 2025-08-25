entity spawn_menu_load(ecs*, const entity);

void button_event_continue_game(
    ecs *world,
    const ClickEventData *event
) {
    entity player = event->clicker;
    zox_geter(player, ElementLinks, elements);
    find_array_element_with_tag(elements, MenuMain, menu);
    if (!menu) {
        zox_log_error("main menu not found")
        return;
    }
    zox_delete(menu);
    spawn_menu_load(world, player);
}


void button_event_load_cancel(
    ecs *world,
    const ClickEventData *event
) {
    entity player = event->clicker;
    zox_geter(player, ElementLinks, elements);
    find_array_element_with_tag(elements, MenuLoad, menu);
    if (!menu) {
        zox_log_error("MenuLoad menu not found")
        return;
    }
    zox_delete(menu);
    spawn_main_menu(world, player, game_name);
}


void button_event_load_confirm(
    ecs *world,
    const ClickEventData *event
) {
    entity player = event->clicker;
    zox_geter(player, ElementLinks, elements);
    find_array_element_with_tag(elements, MenuLoad, menu);
    if (!menu) {
        zox_log_error("main menu not found")
        return;
    }
    zox_delete(menu);
    const entity game = zox_get_value(player, GameLink);
    const entity realm = zox_get_value(game, RealmLink);
    // todo: spawn realm data like voxels/stats/skills when starting new game
    load_realm(game_name, "seed.dat", &realm_save);
    set_noise_seed(realm_save.seed);
    zox_set(realm, Seed, { realm_save.seed });
    zox_set(game, GameStateTarget, { zox_game_load });
}
