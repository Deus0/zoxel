// tst: monster_sight, slimes attack the players they see (ai/sys/sight.c)
//  - two slimes 5 away: one facing the player, one facing away (out of its view cone, past its sense)
//  - the one facing goes into battle with the player as its attacker, unhit; the other stays at peace
//  - run: bash bsh/tst.sh monster_sight
// tst: slime_pounce, a slime's melee is a pounce (skills/sys/pounce.c)
//  - a slime beside the player in battle (1 away, closer than its tentacle) squashes down (its Scale3 under 0.8 high), leaps and hurts the player
//  - the camera freed to its side: tst/slime_pounce/squash.ppm and leap.ppm, to look at
//  - run: bash bsh/tst.sh slime_pounce
// tst: slime_tentacle, a slime whips its tentacle (skills/sys/tentacle.c)
//  - a slime 3 away in battle (past its pounce, in its tentacle's reach): its tentacle's particles come out (its
//    Particle3D children), and the player's health drops while it's out
//  - tst/slime_tentacle/out.ppm and back.ppm, the tentacle at full stretch and drawn back: out.ppm has its green
//    between the slime and the player (its particles drawn, not only spawned)
//  - run: bash bsh/tst.sh slime_tentacle
entity zox_tst_monsters[2];
int zox_tst_tentacle_bits = 0;          // the most particles seen out
float zox_tst_tentacle_before = -1;     // the player's health as it started
float zox_tst_tentacle_after = -1;      // as it ended
byte zox_tst_monsters_stage = 0;
double zox_tst_monsters_time = 0;
float zox_tst_monsters_health = 0;
float zox_tst_monsters_squash = 1;      // the slime's lowest height scale
byte zox_tst_monsters_shots = 0;        // squash, leap

static inline entity zox_tst_realm_slime(ecs* world, entity realm) {
    entity found = 0;
    zox_each_link(world, realm, CharacterLinks, meta) {
        if (zox_has(meta, Monster)) {
            found = meta;
        }
    }
    return found;
}

// a rotation about up whose ahead (quaternion_to_normal) is closest to a direction
static inline float4 zox_tst_facing(float3 direction) {
    float4 best = quaternion_identity;
    float best_dot = -2;
    for (int a = 0; a < 360; a += 5) {
        const float4 q = quaternion_from_euler((float3) { 0, a * degrees_to_radians, 0 });
        const float d = float3_dot(quaternion_to_normal(q), direction);
        if (d > best_dot) {
            best_dot = d;
            best = q;
        }
    }
    return best;
}

static inline float zox_tst_monsters_player_health(ecs* world) {
    const entity health = zox_get_child_by_id(world, zox_tst_player_character(world), zox_id(StatHealth));
    return zox_valid(health) ? zox_getv(health, StatValue) : -1;
}

// spawns count slimes beside the player at offsets, facing ways; 1 once they're ready
static inline byte zox_tst_monsters_spawn(ecs* world, byte count, const float3* offsets, const float3* facings) {
    const entity character = zox_tst_player_character(world);
    if (zox_tst_monsters_stage == 0) {
        const entity realm = zox_get_link(world, character, RealmLink);
        const entity slime = zox_tst_realm_slime(world, realm);
        if (!zox_valid(slime)) {
            zox_tst_check(0, "the realm has no monster character prefab");
            zox_tst_monsters_stage = 9;     // given up
            return 1;
        }
        const float3 p = zox_getv(character, Position3D);
        for (byte i = 0; i < count; i++) {
            zox_tst_monsters[i] = spawn_character3_npc(world, slime, realm, zox_get_link(world, character, TerrainLink),
                21 + i, 0, 0, float3_add(p, offsets[i]), zox_tst_facing(facings[i]), NULL);
            zox_remove(zox_tst_monsters[i], Coward);
            zox_setv(zox_tst_monsters[i], DefaultBehaviour, zox_behaviour_idle);
        }
        zox_tst_monsters_stage = 1;
        return 0;
    }
    zox_tst_waiting = "the slimes to initialize";
    for (byte i = 0; i < count; i++) {
        const entity e = zox_tst_monsters[i];
        if (!zox_valid(e) || zox_has(e, PreInitialize) || zox_has(e, Initialize)) {
            return 0;
        }
    }
    return 1;
}

byte zox_tst_monster_sight_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const float3 offsets[2] = { { 5, 0, 0 }, { -5, 0, 0 } };
    const float3 facings[2] = { { -1, 0, 0 }, { -1, 0, 0 } };    // the first at the player, the second away
    if (zox_tst_monsters_stage == 9) {
        return 1;
    }
    if (zox_tst_monsters_stage < 2) {
        if (zox_tst_monsters_spawn(world, 2, offsets, facings) && zox_tst_monsters_stage == 1) {
            zox_tst_monsters_time = zox_current_time;
            zox_tst_monsters_stage = 2;
        }
        return 0;
    }
    zox_tst_waiting = "the slime facing the player to attack";
    const entity e = zox_tst_monsters[0];
    return (zox_valid(e) && zox_getv(e, CombatState) == zox_combat_battle) || zox_current_time - zox_tst_monsters_time > 5;
}

void zox_tst_monster_sight_check(ecs* world) {
    const entity character = zox_tst_player_character(world);
    const entity seer = zox_tst_monsters[0], blind = zox_tst_monsters[1];
    zox_tst_check(zox_valid(seer) && zox_valid(blind), "a slime is gone");
    if (!zox_valid(seer) || !zox_valid(blind)) {
        return;
    }
    zox_log("[tst] monster_sight facing [%i] attacker [%i], away [%i]", zox_getv(seer, CombatState),
        zox_get_link(world, seer, AttackerLink) == character, zox_getv(blind, CombatState));
    zox_tst_check(zox_getv(seer, CombatState) == zox_combat_battle, "the slime facing the player didn't attack, [%i]", zox_getv(seer, CombatState));
    zox_tst_check(zox_get_link(world, seer, AttackerLink) == character, "the slime's attacker isn't the player");
    zox_tst_check(zox_getv(blind, CombatState) == zox_combat_peace, "the slime facing away attacked, [%i]", zox_getv(blind, CombatState));
}

byte zox_tst_slime_pounce_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const float3 offsets[1] = { { 1, 0, 0 } };      // closer than its tentacle reaches
    const float3 facings[1] = { { -1, 0, 0 } };
    if (zox_tst_monsters_stage == 9) {
        return 1;
    }
    if (zox_tst_monsters_stage < 2) {
        if (zox_tst_monsters_spawn(world, 1, offsets, facings) && zox_tst_monsters_stage == 1) {
            zox_tst_monsters_health = zox_tst_monsters_player_health(world);
            combat_on_hit(world, zox_tst_monsters[0], zox_tst_player_character(world));
            zox_tst_monsters_time = zox_current_time;
            zox_tst_monsters_stage = 2;
        }
        return 0;
    }
    zox_tst_waiting = "the slime to pounce on the player";
    const entity e = zox_tst_monsters[0];
    const entity camera = zox_get_link(world, dbg_player, CameraLink);
    if (zox_valid(e) && zox_valid(camera)) {
        // side on to the slime and the player
        const float3 a = zox_getv(e, Position3D), b = zox_getv(zox_tst_player_character(world), Position3D);
        const float3 mid = float3_scale(float3_add(a, b), 0.5f);
        const float3 eye = float3_add(mid, (float3) { 0, 0.6f, 3.5f });
        if (zox_get_parent(world, camera)) {
            zox_set_parent(world, camera, 0);
        }
        zox_setv(camera, Position3D, eye);
        zox_setv(camera, Rotation3D, camera_look_at(eye, mid));
    }
    if (zox_valid(e) && zox_has(e, Scale3)) {
        const float3 scale = zox_getv(e, Scale3);
        zox_tst_monsters_squash = float_min(zox_tst_monsters_squash, scale.y);
        const byte shot = zox_tst_monsters_shots == 0 && scale.y < 0.65f ? 1 : zox_tst_monsters_shots == 1 && scale.y > 1.2f ? 2 : 0;
        if (shot && !zox_screenshot_request[0]) {
            snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s",
                zox_tst_path(shot == 1 ? "slime_pounce/squash.ppm" : "slime_pounce/leap.ppm"));
            zox_tst_monsters_shots = shot;
        }
    }
    if (zox_screenshot_request[0]) {
        return 0;
    }
    return zox_tst_monsters_player_health(world) < zox_tst_monsters_health || zox_current_time - zox_tst_monsters_time > 15;
}

void zox_tst_slime_pounce_check(ecs* world) {
    const float health = zox_tst_monsters_player_health(world);
    zox_log("[tst] slime_pounce squashed to [%.2f], player health [%.1f] from [%.1f] after [%.1fs]", zox_tst_monsters_squash,
        health, zox_tst_monsters_health, zox_current_time - zox_tst_monsters_time);
    zox_tst_check(zox_tst_monsters_squash < 0.8f, "the slime didn't squash, its height [%.2f]", zox_tst_monsters_squash);
    zox_tst_check(health < zox_tst_monsters_health, "the slime didn't hurt the player in 15s");
}

byte zox_tst_slime_tentacle_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const float3 offsets[1] = { { 3, 0, 0 } };
    const float3 facings[1] = { { -1, 0, 0 } };
    if (zox_tst_monsters_stage == 9) {
        return 1;
    }
    if (zox_tst_monsters_stage < 2) {
        if (zox_tst_monsters_spawn(world, 1, offsets, facings) && zox_tst_monsters_stage == 1) {
            combat_on_hit(world, zox_tst_monsters[0], zox_tst_player_character(world));
            zox_tst_monsters_time = zox_current_time;
            zox_tst_monsters_stage = 2;
        }
        return 0;
    }
    zox_tst_waiting = "the slime to whip its tentacle";
    const entity e = zox_tst_monsters[0];
    if (!zox_valid(e)) {
        return 1;
    }
    const byte out = zox_has(e, SkillUsing) && zox_has(zox_getv(e, SkillUsing), Tentacle);
    if (out) {
        entity bits[tentacle_bits + 8];
        const int count = (int) zox_get_children_by_id(world, e, bits, tentacle_bits + 8, zox_id(Particle3D));
        zox_tst_tentacle_bits = count > zox_tst_tentacle_bits ? count : zox_tst_tentacle_bits;
        if (zox_tst_tentacle_before < 0) {
            zox_tst_tentacle_before = zox_tst_monsters_player_health(world);
        }
        // side on, a shot as it's held out
        const float t = zox_current_time - zox_getv(e, SkillUsedAt);
        const entity camera = zox_get_link(world, dbg_player, CameraLink);
        if (zox_valid(camera)) {
            const float3 a = zox_getv(e, Position3D), b = zox_getv(zox_tst_player_character(world), Position3D);
            const float3 mid = float3_scale(float3_add(a, b), 0.5f);
            const float3 eye = float3_add(mid, (float3) { 0, 0.8f, 3 });
            if (zox_get_parent(world, camera)) {
                zox_set_parent(world, camera, 0);
            }
            zox_setv(camera, Position3D, eye);
            zox_setv(camera, Rotation3D, camera_look_at(eye, mid));
        }
        // held out, then drawn back (its tip curling)
        const double back_at = tentacle_windup + tentacle_out + tentacle_hold + tentacle_back * 0.4;
        if (t > tentacle_windup + tentacle_out && !zox_tst_monsters_shots && !zox_screenshot_request[0]) {
            snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("slime_tentacle/out.ppm"));
            zox_tst_monsters_shots = 1;
        } else if (t > back_at && zox_tst_monsters_shots == 1 && !zox_screenshot_request[0]) {
            snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("slime_tentacle/back.ppm"));
            zox_tst_monsters_shots = 2;
        }
    } else if (zox_tst_tentacle_before >= 0) {
        zox_tst_tentacle_after = zox_tst_monsters_player_health(world);
        return !zox_screenshot_request[0];
    }
    return zox_current_time - zox_tst_monsters_time > 12;
}

// green pixels in the strip between the player (left) and the slime (right), out of a hundred
int zox_tst_tentacle_green(const char* file) {
    int2 size;
    byte* pixels = zox_tst_load_ppm(zox_tst_path(file), &size);
    if (!pixels) {
        return -1;
    }
    int count = 0, total = 0;
    for (int y = size.y * 3 / 8; y < size.y * 13 / 24; y++) {
        for (int x = size.x / 3; x < size.x * 2 / 3; x++, total++) {
            const byte* p = zox_tst_ppm_pixel(pixels, size, x, y);
            count += p[1] > p[0] + 40 && p[1] > p[2] + 30;
        }
    }
    free(pixels);
    return total ? count * 100 / total : 0;
}

void zox_tst_slime_tentacle_check(ecs* world) {
    const int green = zox_tst_tentacle_green("slime_tentacle/out.ppm");
    zox_log("[tst] slime_tentacle bits [%i], player health [%.1f] to [%.1f], green [%i%%]", zox_tst_tentacle_bits,
        zox_tst_tentacle_before, zox_tst_tentacle_after, green);
    zox_tst_check(zox_tst_tentacle_before >= 0, "the slime didn't whip its tentacle in 12s");
    zox_tst_check(zox_tst_tentacle_bits == tentacle_bits, "its tentacle had [%i] particles, not [%i]", zox_tst_tentacle_bits, tentacle_bits);
    zox_tst_check(zox_tst_tentacle_after >= 0 && zox_tst_tentacle_after < zox_tst_tentacle_before, "its tentacle didn't hurt the player");
    zox_tst_check(green >= 2, "its tentacle wasn't drawn: [%i%%] green between the slime and the player", green);
}

void zox_tsts_monsters(ecs* world) {
    zox_tst_add("slime_tentacle", zox_tst_slime_tentacle_ready, zox_tst_slime_tentacle_check, 120);
    zox_tst_set_setup("slime_tentacle", zox_tst_level_setup, 0);
    zox_tst_add("monster_sight", zox_tst_monster_sight_ready, zox_tst_monster_sight_check, 120);
    zox_tst_set_setup("monster_sight", zox_tst_level_setup, 0);
    zox_tst_add("slime_pounce", zox_tst_slime_pounce_ready, zox_tst_slime_pounce_check, 120);
    zox_tst_set_setup("slime_pounce", zox_tst_level_setup, 0);
}
