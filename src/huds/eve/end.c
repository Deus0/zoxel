void main_menu_event_delay(ecs *world, entity e) {
    spawn_main_menu(world, e, game_name);
    entity mouse = zox_get_child_by_id(world, e, zox_id(Mouse));
    if (mouse) {
        zox_setv(mouse, MouseLock, 0);
    } else {
        zox_logw("Player had no mouse [%s]", zox_getn(e));
    }
}

static inline void destroy_canvas_game_uis(ecs* world, entity e, byte dbg_log) {
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

void game_state_end_huds(ecs* world, entity game, byte state) {
    const byte dbg_log = 0;
    const byte delay_end = is_end_game_delays;
    if (state != zox_game_state_the_end) {
        return;
    }
    entity realm = zox_get_child_by_id(world, game, zox_id(Realm));
    play_playlist(world, realm, 0);
    iter it2 = zox_children(world, game);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity player = it2.entities[j];
            if (!zox_has(player, Player)) {
                continue;
            }
            entity canvas = zox_get_link(world, player, CanvasLink);
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
