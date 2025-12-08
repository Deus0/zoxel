// NOTE: WE NOW NEED TO SPAWN TERRAIN CHUNK HERE IF IT DOESN"T EXIST!
//      - streaming breaks if all chunks die..
void player_end_game3_main_menu(
    ecs *world,
    const entity player
) {
    zox_geter_value(player, GameLink, entity, game);
    zox_geter_value(game, RealmLink, entity, realm);
    // music - attach to game from music module
    play_playlist(world, realm, 0);
    // spawn main menu here
    spawn_main_menu(world, player, game_name);
}

void player_end_game(
    ecs *world,
    const entity player,
    const byte is_delays
) {
    disable_inputs_until_release(world, player, zox_device_mode_none, 1);

    zox_geter_value(player, CanvasLink, entity, canvas);
    find_child_with_tag(canvas, MenuPaused, menu_paused);
    find_child_with_tag(canvas, Taskbar, taskbar);
    if (is_delays) {
        trigger_canvas_fade_transition(world, canvas, end_game_delay_fade, 0.8);
    }

    if (zox_valid(local_mouse)) {
        zox_set(local_mouse, MouseLock, { 0 });
    }

    // remove player uis
    if (zox_valid(menu_paused)) {
        zox_delete(menu_paused); // for second player
    }
    if (zox_valid(taskbar)) {
        zox_delete(taskbar);
    }

    if (zox_game_type == zox_game_mode_3D) {
        if (is_delays) {
            delay_event(world, &player_end_game3_main_menu, player, end_game_delay + 0.4f);
        } else {
            player_end_game3(world, player);
        }
    } else if (zox_game_type == zox_game_mode_2D) {
        delay_event(world, &player_end_game2D, player, end_game_delay);
    }
}

// game state implementation for players module
void players_game_state(
    ecs *world,
    const entity game,
    const byte last_state,
    const byte state
) {
    const byte delay_start = is_start_game_delays;
    const byte delay_end = is_end_game_delays;
    zox_geter(game, PlayerLinks, players);
    for (int i = 0; i < players->length; i++) {
        const entity player = players->value[i];
        if (state == zox_game_start) {
            player_end_game(world, player, delay_end);
        } else if (state == zox_game_paused) {
            pause_player(world, player);
        } else if (last_state == zox_game_paused && state == zox_game_playing) {
            resume_player(world, player);
        }
    }
}