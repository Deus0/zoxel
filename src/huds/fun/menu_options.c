entity spawn_main_menu(ecs*, const entity, const char*);

void button_event_menu_main(ecs *world, const ClickEventData event) {
    zox_geter_value(event.clicker, CanvasLink, entity, canvas)
    find_child_with_tag(canvas, MenuOptions, menu)
    if (menu == 0) {
        zox_log_error("options menu not found")
        return;
    }
    zox_delete(menu)
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