void button_event_end_game(ecs *world, ClickEventData event) {
    // TODO: Make this a game state change to end game
    entity game = zox_getv(event.clicker, GameLink);
    if (!zox_valid(game)) {
        zox_loge("Game Invalid in end game");
        return;
    }
    zox_set(game, GameStateTarget, { zox_game_start });
    zox_geter(game, PlayerLinks, players);
    zox_geter_value(game, RealmLink, entity, realm);
    zox_set(game, RealmLink, { 0 });
    if (!zox_valid(realm)) {
        zox_loge("Realm Invalid in end game");
        return;
    }
    /*for (byte i = 0; i < players->length; i++) {
        entity e = players->value[i];
        // Remove game uis
        zox_geter_value(e, CanvasLink, entity, canvas);
        if (!zox_valid(canvas)) {
            continue;
        }
        entity pause_menu = zox_get_child_by_id(world, canvas, zox_id(MenuPaused));
        if (pause_menu) {
            zox_delete(pause_menu);
        }
        entity taskbar = zox_get_child_by_id(world, canvas, zox_id(Taskbar));
        if (taskbar) {
            zox_delete(taskbar);
        }
    }*/
    // Detatch Camera and Deblur
    for (byte i = 0; i < players->length; i++) {
        entity e = players->value[i];
        entity camera = zox_getv(e, CameraLink);
        if (!zox_valid(camera)) {
            continue;
        }
        zox_set_parent(world, camera, 0);
        zox_set(camera, CameraBlur, { 0 });
    }
    // TODO: destroy realm in system when stats is zox_game_end
    /*zox_geter_value(realm, TilemapLink, entity, tilemap);
    if (zox_valid(tilemap)) {
        zox_delete(tilemap);
    }*/
    /*zox_geter_value(realm, TerrainLink, entity, terrain);
    if (zox_valid(terrain)) {
        // zox_loge("Disposing Terrain [%s]", zox_get_name(terrain));
        zox_delete(terrain);
    } else {
        zox_loge("Terrain missing when ending game.");
    }*/
    zox_delete(realm);
    local_terrain = 0;
}

void pause_resume(ecs *world, const entity player) {
    entity game = zox_getv(player, GameLink);
    byte game_state = zox_getv(game, GameState);
    if (!(game_state == zox_game_state_playing || game_state == zox_game_state_paused)) {
        return;
    }
    byte is_paused = game_state == zox_game_state_paused;
    if (is_paused) {
        zox_set(game, GameStateTarget, { zox_game_state_playing });
    }
}

void button_event_return_to_game(ecs *world, ClickEventData event) {
    pause_resume(world, event.clicker);
}
