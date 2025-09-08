entity spawn_menu_load(ecs*, const entity);

void button_event_continue_game(
    ecs *world,
    const ClickEventData event
) {
    entity player = event.clicker;
    zox_geter(player, ElementLinks, elements);
    find_array_element_with_tag(elements, MenuMain, menu);
    if (!menu) {
        zox_log_error("main menu not found")
        return;
    }
    zox_delete(menu);
    spawn_menu_load(world, player);
}