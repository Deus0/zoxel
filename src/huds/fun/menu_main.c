extern entity spawn_menu_options(ecs*, entity, entity, int2, float2);

void button_event_menu_options(ecs *world, ClickEventData event) {
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
    spawn_menu_options(world, event.clicker, canvas, int2_zero, float2_half);
}

void engine_end_delayed(ecs* world, entity e) {
    engine_end();
}

void button_event_exit_app(ecs *world, ClickEventData event) {
    disable_time_pausing();
    // close on all players
    entity player = event.clicker;
    entity game = zox_get_link(world, player, GameLink);
    zox_geter(game, PlayerLinks, players);
    for (int i = 0; i < players->length; i++) {
        entity e = players->value[i];
        entity canvas = zox_get_link(world, e, CanvasLink);
        if (!zox_valid(canvas)) {
            continue;
        }
        entity main_menu = zox_get_child_by_id(world, canvas, zox_id(MenuMain));
        if (main_menu) {
            close_ui_related_tooltip(world, main_menu);
            zox_delete(main_menu);
        }
        trigger_canvas_fade_in(world, canvas, 0, 0.4f);
    }
    delay_event(world, &engine_end_delayed, 0, 2.0f);
}
