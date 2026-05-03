entity spawn_main_menu(ecs*, entity, const char*);

void button_event_menu_main(ecs *world, ClickEventData event) {

    zox_geter_value(event.clicker, CanvasLink, entity, canvas);

    if (!zox_valid(canvas)) {
        return;
    }

    entity options_menu = find_child_with_tag2(world, canvas, zox_id(MenuOptions));
    if (zox_valid(options_menu)) {
        zox_delete(options_menu);
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
