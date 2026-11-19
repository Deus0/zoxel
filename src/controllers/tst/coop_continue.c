// tst: coop_continue, a split screen game continued: each player loads at its own save's position
//  - setup: load_games' saves, the newest given a player.dat and a player2.dat, the players 16 blocks apart
//  - continue -> the newest save -> confirm, with --splitscreen's second player joined
//  - each character spawns over its own save's position (x and z, it may fall after)
//  - they all loaded player.dat, both spawned at the first player's
//  - run: bash bsh/tst.sh coop_continue
const float3 zox_tst_coop_continue_positions[2] = { { 8, 30, 8 }, { 24, 30, 8 } };
float3 zox_tst_coop_continue_spawned[2];

void zox_tst_coop_continue_setup() {
    zox_tst_load_games_setup();
    is_split_screen = 1;
    char folder[640];
    snprintf(folder, sizeof(folder), "%s/realm_%03i", zox_tst_load_saves_path, zox_tst_load_saves - 1);
    const char* files[2] = { "player.dat", "player2.dat" };
    for (int i = 0; i < 2; i++) {
        const SaveDataCharacter save = { .position = zox_tst_coop_continue_positions[i], .euler = float3_zero };
        if (!save_file_struct(folder, files[i], &save, sizeof(SaveDataCharacter))) {
            zox_loge("[tst] couldn't save [%s] in [%s]", files[i], folder);
        }
    }
}

byte zox_tst_coop_continue_ready(ecs* world) {
    if (!zox_tst_continue_game_ready(world)) {
        return 0;
    }
    zox_tst_waiting = "the second player's character";
    if (players_playing < 2) {
        return 0;
    }
    for (int i = 0; i < 2; i++) {
        const entity character = zox_get_link(world, zox_players[i], CharacterLink);
        if (!zox_valid(character)) {
            return 0;
        }
        zox_tst_coop_continue_spawned[i] = zox_getv(character, Position3D);
    }
    return 1;
}

void zox_tst_coop_continue_check(ecs* world) {
    (void) world;
    zox_tst_check(players_playing == 2, "[%i] players, --splitscreen joins a second", players_playing);
    for (int i = 0; i < 2; i++) {
        const float3 saved = zox_tst_coop_continue_positions[i];
        const float3 at = zox_tst_coop_continue_spawned[i];
        const float dx = at.x - saved.x, dz = at.z - saved.z;
        const float distance = sqrtf(dx * dx + dz * dz);
        zox_log("[tst] player [%i] spawned at [%.1fx%.1fx%.1f], saved at [%.1fx%.1fx%.1f]", i + 1, at.x, at.y, at.z,
            saved.x, saved.y, saved.z);
        zox_tst_check(distance < 1, "player [%i] spawned [%.1f] blocks from its save's position", i + 1, distance);
    }
}
