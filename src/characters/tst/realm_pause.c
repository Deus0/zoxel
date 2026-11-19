// tst: realm_pause, pausing the game stops its realm's clock and all in it (timing/sys/clock.c, games game_state_realm_clock)
//  - the player's character lifted a few blocks and pushed sideways, then the game paused (Start, its GameStateTarget)
//  - paused: the realm's ClockScale 0, the terrain's and the character's ClockDelta 0, the character where it was a
//    second later (no fall, no slide)
//  - paused, no npc moves (their ai and physics on the realm's clock)
//  - resumed: its clock runs again, it falls
//  - run: bash bsh/tst.sh realm_pause
int zox_tst_rp_stage = 0;
double zox_tst_rp_time = 0;
float3 zox_tst_rp_at;
#define zox_tst_rp_npcs_max 64
entity zox_tst_rp_npcs[zox_tst_rp_npcs_max];
float3 zox_tst_rp_npc_at[zox_tst_rp_npcs_max];
int zox_tst_rp_npcs_count = 0;
float zox_tst_rp_npc_moved = -1;    // the most an npc moved paused

float zox_tst_rp_scale = -1, zox_tst_rp_terrain_delta = -1, zox_tst_rp_delta = -1, zox_tst_rp_moved = -1, zox_tst_rp_fell = -1;

byte zox_tst_realm_pause_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity player = zox_players[0];
    const entity game = zox_get_link(world, player, GameLink);
    const entity character = zox_tst_player_character(world);
    const entity terrain = zox_get_link(world, character, TerrainLink);
    const entity realm = zox_get_link(world, game, RealmLink);
    const double t = zox_current_time - zox_tst_rp_time;
    zox_tst_progress(zox_tst_rp_stage * 100 + t * 10);
    switch (zox_tst_rp_stage) {
        case 0: {
            zox_tst_waiting = "lifted, then paused";
            const float block = zox_getv(terrain, BlockScale);
            float3 p = zox_getv(character, Position3D);
            p.y += 4 * block;
            zox_setv(character, Position3D, p);
            zox_setv(character, Velocity3D, ((float3) { 2 * block, 0, 0 }));
            zox_setv(game, GameStateTarget, zox_game_state_paused);
            zox_tst_rp_stage = 1;
            zox_tst_rp_time = zox_current_time;
            return 0;
        }
        case 1:
            zox_tst_waiting = "the game paused";
            if (zox_getv(game, GameState) != zox_game_state_paused || t < 0.3) {
                return t > 10;
            }
            zox_tst_rp_at = zox_getv(character, Position3D);
            // the npcs (characters but the player's) where they are (wandering, attacking: their ai and physics on the realm's clock)
            {
                ecs_iter_t itn = ecs_each_id(world, zox_id(Character));
                while (ecs_each_next(&itn)) {
                    for (int j = 0; j < itn.count && zox_tst_rp_npcs_count < zox_tst_rp_npcs_max; j++) {
                        if (itn.entities[j] != character && zox_has(itn.entities[j], Position3D)) {
                            zox_tst_rp_npcs[zox_tst_rp_npcs_count] = itn.entities[j];
                            zox_tst_rp_npc_at[zox_tst_rp_npcs_count++] = zox_getv(itn.entities[j], Position3D);
                        }
                    }
                }
            }
            zox_tst_rp_stage = 2;
            zox_tst_rp_time = zox_current_time;
            return 0;
        case 2:
            zox_tst_waiting = "a second paused";
            if (t < 1) {
                return 0;
            }
            zox_tst_rp_scale = zox_getv(realm, ClockScale);
            zox_tst_rp_terrain_delta = zox_getv(terrain, ClockDelta);
            zox_tst_rp_delta = zox_getv(character, ClockDelta);
            zox_tst_rp_moved = float3_distance(zox_getv(character, Position3D), zox_tst_rp_at);
            zox_tst_rp_npc_moved = 0;
            for (int j = 0; j < zox_tst_rp_npcs_count; j++) {
                if (zox_valid(zox_tst_rp_npcs[j])) {
                    zox_tst_rp_npc_moved = float_max(zox_tst_rp_npc_moved,
                        float3_distance(zox_getv(zox_tst_rp_npcs[j], Position3D), zox_tst_rp_npc_at[j]));
                }
            }
            zox_tst_rp_at = zox_getv(character, Position3D);
            zox_setv(game, GameStateTarget, zox_game_state_playing);
            zox_tst_rp_stage = 3;
            zox_tst_rp_time = zox_current_time;
            return 0;
        case 3:
            zox_tst_waiting = "resumed, falling";
            if (t < 1) {
                return 0;
            }
            zox_tst_rp_fell = zox_tst_rp_at.y - zox_getv(character, Position3D).y;
            return 1;
    }
    return 1;
}

void zox_tst_realm_pause_check(ecs* world) {
    (void) world;
    zox_log("[tst] realm_pause: paused the realm's scale [%.2f], the terrain's delta [%.4f], the character's [%.4f], it moved [%.4f]; resumed it fell [%.3f]",
        zox_tst_rp_scale, zox_tst_rp_terrain_delta, zox_tst_rp_delta, zox_tst_rp_moved, zox_tst_rp_fell);
    zox_tst_check(zox_tst_rp_scale == 0, "paused, the realm's clock scale is [%.2f], not 0", zox_tst_rp_scale);
    zox_tst_check(zox_tst_rp_terrain_delta == 0, "paused, the terrain's clock moved [%.4f]", zox_tst_rp_terrain_delta);
    zox_tst_check(zox_tst_rp_delta == 0, "paused, the character's clock moved [%.4f]", zox_tst_rp_delta);
    zox_tst_check(zox_tst_rp_moved >= 0 && zox_tst_rp_moved < 0.001f, "paused, the character moved [%.4f]", zox_tst_rp_moved);
    zox_log("[tst] realm_pause: [%i] npcs, the most one moved paused [%.4f]", zox_tst_rp_npcs_count, zox_tst_rp_npc_moved);
    zox_tst_check(zox_tst_rp_npc_moved < 0.001f, "paused, an npc moved [%.4f]", zox_tst_rp_npc_moved);
    zox_tst_check(zox_tst_rp_fell > 0.01f, "resumed, the character didn't fall [%.3f]", zox_tst_rp_fell);
}

void zox_tsts_realm_pause(ecs* world) {
    zox_tst_add("realm_pause", zox_tst_realm_pause_ready, zox_tst_realm_pause_check, 90);
}
