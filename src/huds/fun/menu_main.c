extern entity spawn_menu_options(ecs*, entity, entity, int2, float2);

void button_event_menu_options(ecs *world, ClickEventData event) {

    zox_geter_value(event.clicker, CanvasLink, entity, canvas);

    entity main_menu = find_child_with_tag_recursive(world, canvas, zox_id(MenuMain));

    if (!zox_valid(main_menu)) {
        zox_loge("No MenuMain found on canvas");
    } else {
        zox_delete(main_menu);
    }

    spawn_menu_options(world, event.clicker, canvas, int2_zero, float2_half);
}

void engine_end_delayed(ecs* world, const entity e) {
    (void) world;
    (void) e;
    engine_end();
}

void button_event_exit_app(ecs *world, ClickEventData event) {
    disable_time_pausing();

    // close on all players
    entity game = zox_get_value(event.clicker, GameLink);

    zox_geter(game, PlayerLinks, players);

    for (int i = 0; i < players->length; i++) {
        entity e = players->value[i];
        entity canvas = zox_get_value(e, CanvasLink);

        entity menu = find_child_with_tag_recursive(world, canvas, zox_id(MenuMain));
        if (menu) {
            zox_delete(menu);
        }

        trigger_canvas_fade_in(world, canvas, 0, 0.4f);
    }
    delay_event(world, &engine_end_delayed, 0, 2.0f);
}
