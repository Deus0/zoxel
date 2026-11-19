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

// tst: coop_join_continue, a saved game continued with a new second player (only player.dat): it starts at a home in
//  the town closest to the first (the first loaded, picked no town: the second picked a random one)
//  - run: bash bsh/tst.sh coop_join_continue
void zox_tst_coop_join_continue_setup() {
    zox_tst_load_games_setup();
    terrain_sea_height = 0;     // the sea lowered: towns keep off water, most of the world was under it (no towns)
    is_split_screen = 1;
    char folder[640];
    snprintf(folder, sizeof(folder), "%s/realm_%03i", zox_tst_load_saves_path, zox_tst_load_saves - 1);
    const SaveDataCharacter save = { .position = zox_tst_coop_continue_positions[0], .euler = float3_zero };
    save_file_struct(folder, "player.dat", &save, sizeof(SaveDataCharacter));
}

// the town closest to a position (x and z, in blocks), of the terrain's
static inline entity zox_tst_closest_town(ecs* world, entity terrain, float3 position) {
    const float scale = zox_getv(terrain, BlockScale);
    entity closest = 0;
    float best = 0;
    entity regions[zox_children_capacity];
    const uint regions_count = zox_get_children_by_id(world, terrain, regions, zox_children_capacity, zox_id(Region));
    for (uint r = 0; r < regions_count; r++) {
        entity towns[zox_children_capacity];
        const uint count = zox_get_children_by_id(world, regions[r], towns, zox_children_capacity, zox_id(Town));
        for (uint t = 0; t < count; t++) {
            const int2 p = zox_getv(towns[t], BlockPosition2);
            const float dx = p.x * scale - position.x, dz = p.y * scale - position.z;
            if (!closest || dx * dx + dz * dz < best) {
                best = dx * dx + dz * dz;
                closest = towns[t];
            }
        }
    }
    return closest;
}

void zox_tst_coop_join_continue_check(ecs* world) {
    zox_tst_check(players_playing == 2, "[%i] players, --splitscreen joins a second", players_playing);
    const entity terrain = local_terrain;
    const entity town = zox_tst_closest_town(world, terrain, zox_tst_coop_continue_spawned[0]);
    zox_tst_check(zox_valid(town), "no town in the world (the test can't tell)");
    float distance = 0;
    const entity home = zox_tst_coop_closest_home(world, terrain, zox_tst_coop_continue_spawned[1], &distance);
    zox_log("[tst] the first at [%.1fx%.1f], its closest town [%s]; the second at [%.1fx%.1f], [%.1f] from [%s] of [%s]",
        zox_tst_coop_continue_spawned[0].x, zox_tst_coop_continue_spawned[0].z, zox_get_name(town),
        zox_tst_coop_continue_spawned[1].x, zox_tst_coop_continue_spawned[1].z, distance, zox_get_name(home),
        zox_get_name(zox_get_parent(world, home)));
    zox_tst_check(distance < 1.5f, "the second player isn't at a home's door, [%.1f] from one", distance);
    zox_tst_check(zox_valid(home) && zox_get_parent(world, home) == town, "the second player isn't in the first's closest town");
}
