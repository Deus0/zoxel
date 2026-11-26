// tst: player_states, the first player's states in order, each change once, on its core Dirty (players/sys/event.c)
//  - --autostart's load to playing, the game paused and playing again, then the end, back to the main menu
//  - each state once, including starting while the terrain loads (player_begin_system re-triggered it every frame)
//  - an observer on the player's Dirty records each state as it fires: each one differs from the last (no repeats)
//  - run: bash bsh/tst.sh player_states
#define zox_tst_ps_max 32
byte zox_tst_ps_states[zox_tst_ps_max];
int zox_tst_ps_count = 0;
int zox_tst_ps_stage = 0;
double zox_tst_ps_time = 0;
double zox_tst_ps_since = 0;    // when the player reached the state a step waits for

// every first player state change, as observers see it
void zox_tst_player_states_event(iter* it) {
    zox_sys_begin();
    zox_sys_in(PlayerState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerState, state);
        if (e == zox_players[0] && zox_tst_ps_count < zox_tst_ps_max) {
            zox_tst_ps_states[zox_tst_ps_count++] = state->value;
        }
    }
}

// sets the game's target, once the player has been in wait_for 0.3s (sooner, observers would miss it: it changes
//  before its Dirty fires)
static inline byte zox_tst_ps_step(ecs* world, byte player_state, byte wait_for, byte game_target) {
    if (player_state != wait_for) {
        zox_tst_ps_since = 0;
        return 0;
    }
    if (!zox_tst_ps_since) {
        zox_tst_ps_since = zox_current_time;
    }
    if (zox_current_time - zox_tst_ps_since < 0.3) {
        return 0;
    }
    zox_tst_ps_since = 0;
    zox_setv(local_game, GameStateTarget, game_target);
    zox_tst_ps_stage++;
    zox_tst_ps_time = zox_current_time;
    return 1;
}

byte zox_tst_player_states_ready(ecs* world) {
    if (zox_tst_ps_stage == 0 && !zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const byte state = zox_getv(zox_players[0], PlayerState);
    const double t = zox_current_time - zox_tst_ps_time;
    zox_tst_progress(zox_tst_ps_stage * 100 + zox_tst_ps_count);
    switch (zox_tst_ps_stage) {
        case 0:
            zox_tst_waiting = "playing, then paused";
            zox_tst_ps_step(world, state, zox_player_state_playing, zox_game_state_paused);
            return 0;
        case 1:
            zox_tst_waiting = "the player paused";
            if (!zox_tst_ps_step(world, state, zox_player_state_paused, zox_game_state_playing)) {
                return t > 10;
            }
            return 0;
        case 2:
            zox_tst_waiting = "the player playing again";
            if (!zox_tst_ps_step(world, state, zox_player_state_playing, zox_game_state_the_end)) {
                return t > 10;
            }
            return 0;
        case 3:
            zox_tst_waiting = "the player back in the main menu";
            return state == zox_player_state_main_menu || t > 20;
    }
    return 1;
}

void zox_tst_player_states_check(ecs* world) {
    (void) world;
    const byte expected[] = {
        zox_player_state_loading,
        zox_player_state_new,
        zox_player_state_starting,
        zox_player_state_play_trigger,
        zox_player_state_play_begin,
        zox_player_state_playing,
        zox_player_state_pause_begin,
        zox_player_state_paused,
        zox_player_state_resume_begin,
        zox_player_state_play_trigger,
        zox_player_state_play_begin,
        zox_player_state_playing,
        zox_player_state_the_end,
        zox_player_state_main_menu_begin,
        zox_player_state_main_menu,
    };
    const int expected_count = sizeof(expected) / sizeof(expected[0]);
    char text[256] = "";
    for (int i = 0; i < zox_tst_ps_count; i++) {
        char number[8];
        snprintf(number, sizeof(number), "%s%i", i ? " " : "", zox_tst_ps_states[i]);
        strncat(text, number, sizeof(text) - strlen(text) - 1);
    }
    zox_log("[tst] player_states: [%i] changes [%s]", zox_tst_ps_count, text);
    zox_tst_check(zox_tst_ps_count == expected_count, "[%i] state changes, expected [%i]", zox_tst_ps_count, expected_count);
    for (int i = 0; i < zox_tst_ps_count && i < expected_count; i++) {
        zox_tst_check(zox_tst_ps_states[i] == expected[i], "change [%i] is state [%i], expected [%i]",
            i, zox_tst_ps_states[i], expected[i]);
    }
    for (int i = 1; i < zox_tst_ps_count; i++) {
        zox_tst_check(zox_tst_ps_states[i] != zox_tst_ps_states[i - 1], "change [%i] repeats state [%i]",
            i, zox_tst_ps_states[i]);
    }
}

void zox_tsts_player_states(ecs* world) {
    zox_on_add(zox_tst_player_states_event, [in] players.PlayerState, [none] players.Player, [none] core.Dirty);
    zox_tst_add("player_states", zox_tst_player_states_ready, zox_tst_player_states_check, 120);
}
