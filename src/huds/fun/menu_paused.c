void button_event_end_game(ecs *world, ClickEventData event) {
    entity game = zox_get_value(event.clicker, GameLink);
    zox_geter(game, PlayerLinks, players);

    for (byte i = 0; i < players->length; i++) {
        entity e = players->value[i];
        entity canvas = zox_get_value(e, CanvasLink);

        // find_child_with_tag(canvas, MenuPaused, menu_paused);
        entity menu = find_child_with_tag_recursive(world, canvas, zox_id(MenuPaused));
        if (menu) {
            zox_delete(menu);
        }

        // find_child_with_tag(canvas, Taskbar, taskbar);
        entity taskbar = find_child_with_tag_recursive(world, canvas, zox_id(Taskbar));
        if (taskbar) {
            zox_delete(taskbar);
        }
    }

    // TODO: destroy realm in system when stats is zox_game_end
    zox_geter_value(game, RealmLink, entity, realm);
    if (zox_valid(realm)) {

        zox_geter_value(realm, TerrainLink, entity, terrain);

        if (zox_valid(terrain)) {
            zox_delete(terrain);
        }
        local_terrain = 0;

        zox_geter_value(realm, TilemapLink, entity, tilemap);
        if (zox_valid(tilemap)) {
            zox_delete(tilemap);
        }


        zox_delete(realm);

        zox_set(game, RealmLink, { 0 });

    } else {
        zox_log_error("Realm Missing.");
    }

    zox_set(game, GameStateTarget, { zox_game_start });
}

void button_event_return_to_game(ecs *world, ClickEventData event) {
    pause_resume(world, event.clicker);
}
