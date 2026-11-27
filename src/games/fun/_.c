#include "game_events.c"

static inline entity get_player_game(ecs* world, const entity player) {
    return zox_get_link(world, player, GameLink);
}

/*void dispose_games(ecs* world, void *ctx) {
    (void) world;
    (void) ctx;
    dispose_events_games();
}*/

// paused, the realm's clock stops (its ClockScale 0), all under it with it (timing/sys/clock.c); playing it runs again
void game_realm_clock(ecs* world, entity game, byte state) {
    const entity realm = zox_get_link(world, game, RealmLink);
    if (!zox_valid(realm) || !zox_has(realm, ClockScale)) {
        return;
    }
    if (state == zox_game_state_paused) {
        zox_setv(realm, ClockScale, 0);
    } else if (state == zox_game_state_playing) {
        zox_setv(realm, ClockScale, 1);
    }
}

// the game paused or plays (its core Dirty): its realm's clock stops or runs
void game_state_realm_clock(iter* it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GameState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GameState, state);
        game_realm_clock(world, e, state->value);
    }
}
