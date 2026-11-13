// tst: player_respawn, the player dies and respawns, alive and at full health
//  - waits for the player to be playing, then kills it (its health stat to 0)
//  - waits for the player to leave playing (dying), then to play again after the 16s death screen (respawn.c)
//  - its character is valid, not Dead, its health back at its max
//  - first drafted by the local AI (opencode), its waits and stat names fixed
//  - run: bash bsh/tst.sh player_respawn
byte zox_tst_player_spawn_ready(ecs* world);   // in _.c, after this is included
byte zox_tst_respawn_stage = 0;
int zox_tst_respawn_frames = 0;
double zox_tst_respawn_time = 0;
entity zox_tst_respawn_old_character = 0;
#define zox_tst_respawn_seconds 40.0     // the death screen is 16s (player_state_system), then a spawn

byte zox_tst_player_respawn_ready(ecs* world) {
    if (zox_tst_respawn_stage == 0 && !zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity player = dbg_player;
    const entity character = zox_get_link(world, player, CharacterLink);
    const byte state = zox_getv(player, PlayerState);
    zox_tst_respawn_frames++;
    if (zox_tst_respawn_stage == 0) {
        zox_tst_waiting = "playing";
        if (state != zox_player_state_playing || zox_tst_respawn_frames < 10) {
            return 0;
        }
        const entity health = zox_get_child_by_id(world, character, zox_id(StatHealth));
        zox_tst_check(zox_valid(health), "the player's character has no health");
        if (!zox_valid(health)) {
            return 1;
        }
        zox_setv(health, StatValue, 0);
        zox_tst_respawn_old_character = character;
        zox_tst_respawn_time = zox_current_time;
        zox_tst_respawn_stage = 1;
        return 0;
    }
    // the death screen waits 16s, progress is its state and seconds (the timeout above it catches a hang)
    zox_tst_progress(state * 1000 + (int) (zox_current_time - zox_tst_respawn_time));
    if (zox_current_time - zox_tst_respawn_time > zox_tst_respawn_seconds) {
        return 1;   // timed out, the check says where
    }
    if (zox_tst_respawn_stage == 1) {
        zox_tst_waiting = "the player to die";
        if (state == zox_player_state_playing) {
            return 0;
        }
        zox_log("[tst] the player died, state [%i] after [%.1fs]", state, zox_current_time - zox_tst_respawn_time);
        zox_tst_respawn_stage = 2;
        return 0;
    }
    zox_tst_waiting = "the player to respawn";
    return state == zox_player_state_playing && zox_valid(character) && !zox_has(character, Dead);
}

void zox_tst_player_respawn_check(ecs* world) {
    const entity player = dbg_player;
    const entity character = zox_get_link(world, player, CharacterLink);
    zox_tst_check(zox_tst_respawn_stage == 2, "the player never died (stage [%i])", zox_tst_respawn_stage);
    zox_tst_check(zox_getv(player, PlayerState) == zox_player_state_playing, "the player isn't playing again");
    zox_tst_check(zox_valid(character), "the player has no character");
    if (!zox_valid(character)) {
        return;
    }
    zox_tst_check(!zox_has(character, Dead), "the character is still Dead");
    const entity health = zox_get_child_by_id(world, character, zox_id(StatHealth));
    const float value = zox_valid(health) ? zox_getv(health, StatValue) : -1;
    const float max = zox_valid(health) ? zox_getv(health, StatValueMax) : -1;
    zox_log("[tst] respawned after [%.1fs], character [%s] was [%s], health [%.1f] of [%.1f]",
        zox_current_time - zox_tst_respawn_time, zox_get_name(character),
        zox_get_name(zox_tst_respawn_old_character), value, max);
    zox_tst_check(value > 0 && value >= max, "health [%.1f] of [%.1f] after respawning", value, max);
}
