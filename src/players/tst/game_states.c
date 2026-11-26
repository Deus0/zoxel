// tst: game_states, the game's states in order, each change once, on the core Dirty (games/sys/state.c)
//  - in players: it checks the players follow (players/sys/game.c)
//  - boot (the first screen up), then --autostart's load: start, fading, faded, realm, terrain, loading, end, playing
//  - paused (the realm's clock stops, the player pauses), playing again, then the end, the clean, none (the menu)
//  - an observer on the game's Dirty records each state as it fires: each one differs from the last (no repeats), and
//    its LastGameState is the one before
//  - run: bash bsh/tst.sh game_states
#define zox_tst_gs_max 32
byte zox_tst_gs_states[zox_tst_gs_max];
byte zox_tst_gs_lasts[zox_tst_gs_max];
int zox_tst_gs_count = 0;
int zox_tst_gs_stage = 0;
double zox_tst_gs_time = 0;
float zox_tst_gs_paused_scale = -1;
byte zox_tst_gs_paused_player = 0;

// every game state change, as observers see it
void zox_tst_game_states_event(iter* it) {
    zox_sys_begin();
    zox_sys_in(GameState);
    zox_sys_in(LastGameState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(GameState, state);
        zox_sys_i(LastGameState, last);
        if (zox_tst_gs_count < zox_tst_gs_max) {
            zox_tst_gs_states[zox_tst_gs_count] = state->value;
            zox_tst_gs_lasts[zox_tst_gs_count++] = last->value;
        }
    }
}

byte zox_tst_game_states_ready(ecs* world) {
    if (zox_tst_gs_stage == 0 && !zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity game = local_game;
    const byte state = zox_getv(game, GameState);
    const double t = zox_current_time - zox_tst_gs_time;
    zox_tst_progress(zox_tst_gs_stage * 100 + zox_tst_gs_count);
    switch (zox_tst_gs_stage) {
        case 0:
            zox_tst_waiting = "playing, then paused";
            zox_setv(game, GameStateTarget, zox_game_state_paused);
            zox_tst_gs_stage = 1;
            zox_tst_gs_time = zox_current_time;
            return 0;
        case 1:
            zox_tst_waiting = "the game paused";
            if (state != zox_game_state_paused || t < 0.3) {
                return t > 10;
            }
            {
                const entity realm = zox_get_link(world, game, RealmLink);
                zox_tst_gs_paused_scale = zox_valid(realm) ? zox_getv(realm, ClockScale) : -1;
                const byte player_state = zox_getv(zox_players[0], PlayerState);
                zox_tst_gs_paused_player = player_state == zox_player_state_pause_begin ||
                    player_state == zox_player_state_paused;
            }
            zox_setv(game, GameStateTarget, zox_game_state_playing);
            zox_tst_gs_stage = 2;
            zox_tst_gs_time = zox_current_time;
            return 0;
        case 2:
            zox_tst_waiting = "playing again";
            if (state != zox_game_state_playing || t < 0.3) {
                return t > 10;
            }
            zox_setv(game, GameStateTarget, zox_game_state_the_end);
            zox_tst_gs_stage = 3;
            zox_tst_gs_time = zox_current_time;
            return 0;
        case 3:
            zox_tst_waiting = "the end, back to none (the menu)";
            if (state != zox_game_none) {
                return t > 20;
            }
            return 1;
    }
    return 1;
}

void zox_tst_game_states_check(ecs* world) {
    (void) world;
    const byte expected[] = {
        zox_game_boot,
        zox_game_load_start,
        zox_game_load_fading,
        zox_game_load_faded,
        zox_game_loading_realm,
        zox_game_terrain_spawn,
        zox_game_loading_terrain,
        zox_game_load_end,
        zox_game_state_playing,
        zox_game_state_paused,
        zox_game_state_playing,
        zox_game_state_the_end,
        zox_game_state_the_clean,
        zox_game_none,
    };
    const int expected_count = sizeof(expected) / sizeof(expected[0]);
    char text[256] = "";
    for (int i = 0; i < zox_tst_gs_count; i++) {
        char number[8];
        snprintf(number, sizeof(number), "%s%i", i ? " " : "", zox_tst_gs_states[i]);
        strncat(text, number, sizeof(text) - strlen(text) - 1);
    }
    zox_log("[tst] game_states: [%i] changes [%s]", zox_tst_gs_count, text);
    zox_tst_check(zox_tst_gs_count == expected_count, "[%i] state changes, expected [%i]", zox_tst_gs_count, expected_count);
    for (int i = 0; i < zox_tst_gs_count && i < expected_count; i++) {
        zox_tst_check(zox_tst_gs_states[i] == expected[i], "change [%i] is state [%i], expected [%i]",
            i, zox_tst_gs_states[i], expected[i]);
        if (i > 0) {
            zox_tst_check(zox_tst_gs_lasts[i] == zox_tst_gs_states[i - 1], "change [%i]'s last state is [%i], not [%i]",
                i, zox_tst_gs_lasts[i], zox_tst_gs_states[i - 1]);
        }
    }
    zox_tst_check(zox_tst_gs_paused_scale == 0, "paused, the realm's clock scale is [%.2f], not 0", zox_tst_gs_paused_scale);
    zox_tst_check(zox_tst_gs_paused_player, "paused, the player isn't paused");
}

void zox_tsts_game_states(ecs* world) {
    zox_on_add(zox_tst_game_states_event, [in] games.GameState, [in] games.LastGameState, [none] games.Game,
        [none] core.Dirty);
    zox_tst_add("game_states", zox_tst_game_states_ready, zox_tst_game_states_check, 120);
}
