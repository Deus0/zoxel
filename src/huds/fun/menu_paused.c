void button_event_end_game(ecs *world, ClickEventData event) {

    // TODO: Make this a game state change to end game

    entity game = zox_get_value(event.clicker, GameLink);
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


    for (byte i = 0; i < players->length; i++) {
        entity e = players->value[i];

        // Remove game uis
        zox_geter_value(e, CanvasLink, entity, canvas);

        if (!zox_valid(canvas)) {
            continue;
        }

        entity menu = get_canvas_window(world, canvas, zox_window_paused);
        if (menu) {
            zox_delete(menu);
        }
        entity taskbar = get_canvas_window(world, canvas, zox_window_taskbar);
        if (taskbar) {
            zox_delete(taskbar);
        }
    }

    // Detatch Camera and Deblur
    for (byte i = 0; i < players->length; i++) {
        entity e = players->value[i];

        zox_geter_value(e, CameraLink, entity, camera);

        if (!zox_valid(camera)) {
            continue;
        }

        zox_set_parent(world, camera, 0);
        //zox_geter_value(camera, ParentLink, entity, old_parent);
        //zox_muter(old_parent, Children, old_parent_children);
        //remove_from_Children(old_parent_children, camera);
        //zox_set(camera, ParentLink, { 0 });
        zox_set(camera, CameraBlur, { 0 });
    }

    // TODO: destroy realm in system when stats is zox_game_end

    zox_geter_value(realm, TilemapLink, entity, tilemap);
    if (zox_valid(tilemap)) {
        zox_delete(tilemap);
    }

    zox_geter_value(realm, TerrainLink, entity, terrain);
    if (zox_valid(terrain)) {
        // zox_loge("Disposing Terrain [%s]", zox_get_name(terrain));
        zox_delete(terrain);
    } else {
        zox_loge("Terrain missing when ending game.");
    }

    zox_delete(realm);

    local_terrain = 0;
}

void button_event_return_to_game(ecs *world, ClickEventData event) {
    pause_resume(world, event.clicker);
}
