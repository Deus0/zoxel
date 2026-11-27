void main_menu_event_delay(ecs *world, entity e) {
    // joined players have no menus, their screens go (local coop)
    if (e != zox_players[0]) {
        return;
    }
    end_split_screen(world);
    spawn_main_menu(
        world,
        e,
        game_name);
    entity mouse = zox_get_child_by_id(world, e, zox_id(Mouse));
    if (mouse) {
        zox_setv(mouse, MouseLock, 0);
    } else {
        zox_logw("Player had no mouse [%s]", zox_getn(e));
    }
}

static inline void destroy_canvas_game_uis(
    ecs* world,
    entity e,
    byte dbg_log)
{
    if (dbg_log) {
        zox_log("Destroying Game UI [%s]", zox_getn(e));
    }
    iter it2 = zox_children(world, e);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity e2 = it2.entities[j];
            // TODO: A game ui tag
            if (!zox_has(e2, Window) && !zox_has(e2, Crosshair)) {
                continue;
            }
            if (dbg_log) {
                zox_log("   - UI [%s]", zox_getn(e2));
            }
            zox_delete(e2); // for second player
        }
    }
}

void game_state_end_huds(
    ecs* world,
    entity game,
    byte state)
{
    const byte dbg_log = 0;
    const byte delay_end = is_end_game_delays;
    if (state != zox_game_state_the_end) {
        return;
    }
    entity realm = zox_get_link(world, game, RealmLink);
    if (!zox_valid(realm)) {
        zox_loge("[game_state_end_huds] Invalid [realm]");
        return;
    }
    // its time played this session, last played now, and its clock (its time of day) (realm.txt)
    if (zox_has(realm, FolderPath) && zox_get(realm, FolderPath)->value[0]) {
        realm_info_played(zox_get(realm, FolderPath)->value, zox_current_time, zox_has(realm, ClockTime) ? zox_getv(realm, ClockTime) : -1);
    }
    realm_played_since = 0;
    play_menu_music(world, game);
    iter it2 = zox_children(world, game);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity player = it2.entities[j];
            if (!zox_has(player, Player)) {
                continue;
            }
            entity canvas = player_own_canvas(world, player);
            destroy_canvas_game_uis(world, canvas, dbg_log);
            // FadeOut
            if (delay_end) {
                trigger_canvas_fade_transition(
                    world,
                    canvas,
                    end_game_delay_fade,
                    0.8);
            }
            delay_event(
                world,
                &main_menu_event_delay,
                player,
                2);
        }
    }
}
