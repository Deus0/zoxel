entity spawn_main_menu(ecs*, entity, const char*);

void button_event_menu_main(ecs *world, ClickEventData event) {

    zox_geter_value(event.clicker, CanvasLink, entity, canvas)
    // find_child_with_tag(canvas, MenuOptions, menu);
    entity menu = find_child_with_tag_recursive(world, canvas, zox_id(MenuOptions));

    if (menu) {
        zox_delete(menu);
    }

    spawn_main_menu(world, event.clicker, game_name);
}

void button_event_volume_increase(ecs *world, const ClickEventData event) {
    (void) world;
    (void) event;

    if (master_volume < 1.0f) {
        master_volume += master_volume_increment;
        zox_log(" > increased the volume to [%f]", master_volume)
    }
}

void button_event_volume_decrease(ecs *world, const ClickEventData event) {
    (void) world;
    (void) event;

    if (master_volume > 0) {
        master_volume -= master_volume_increment;
        zox_log(" > decreased the volume to [%f]", master_volume)
    }
}
