// tst: npc_attacks, an npc the player hits fights back and hurts the player
//  - spawns a humanoid npc 5 away from the player (not a coward), hits it as the player would (combat_on_hit)
//  - it goes into battle, chases and attacks (Behaviour), triggers its skill, and the player's health drops
//  - the player strafes at a walk (zox_tst_attacks_strafe, its velocity): the npc aims at its threat (RaycastAim), 5
//    away it shoots (a humanoid), leading the player by its velocity
//  - logs where the chain stops: battle, behaviour, skill, its ray's hit, the player's health
//  - run: bash bsh/tst.sh npc_attacks
byte zox_tst_attacks_stage = 0;
int zox_tst_attacks_frames = 0;
double zox_tst_attacks_time = 0;
entity zox_tst_attacks_npc = 0;
float zox_tst_attacks_health = 0;
float zox_tst_attacks_moved = 0;        // the player's strafing, its furthest from where it started
float3 zox_tst_attacks_start;
#define zox_tst_attacks_seconds 15.0
#define zox_tst_attacks_strafe 4.0f   // the player side to side, units a second, turning every 2s

static inline float zox_tst_attacks_player_health(ecs* world, entity character) {
    const entity health = zox_get_child_by_id(world, character, zox_id(StatHealth));
    return zox_valid(health) ? zox_getv(health, StatValue) : -1;
}

byte zox_tst_npc_attacks_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity player = dbg_player;
    const entity character = zox_get_link(world, player, CharacterLink);
    zox_tst_attacks_frames++;
    if (zox_tst_attacks_stage == 0) {
        zox_tst_waiting = "playing";
        if (zox_getv(player, PlayerState) != zox_player_state_playing || zox_tst_attacks_frames < 10) {
            return 0;
        }
        const entity realm = zox_get_link(world, character, RealmLink);
        const entity terrain = zox_get_link(world, character, TerrainLink);
        const entity humanoid = zox_tst_realm_humanoid(world, realm);
        if (!zox_valid(humanoid)) {
            zox_tst_check(0, "the realm has no humanoid character prefab");
            return 1;
        }
        float3 position = zox_getv(character, Position3D);
        position.x += 5.0f;
        zox_tst_attacks_npc = spawn_character3_npc(world, humanoid, realm, terrain, 7, 0, 0, position, quaternion_identity, NULL);
        zox_remove(zox_tst_attacks_npc, Coward);
        zox_tst_attacks_stage = 1;
        zox_tst_attacks_frames = 0;
        return 0;
    }
    if (zox_tst_attacks_stage == 1) {
        zox_tst_waiting = "the npc to initialize";
        const entity npc = zox_tst_attacks_npc;
        if (zox_tst_attacks_frames < 30 || !zox_valid(npc) || zox_has(npc, PreInitialize) || zox_has(npc, Initialize)) {
            return 0;
        }
        zox_tst_attacks_health = zox_tst_attacks_player_health(world, character);
        combat_on_hit(world, npc, character);
        zox_tst_attacks_time = zox_current_time;
        zox_tst_attacks_start = zox_getv(character, Position3D);
        zox_tst_attacks_stage = 2;
        return 0;
    }
    zox_tst_waiting = "the npc to hurt the player";
    const entity npc = zox_tst_attacks_npc;
    float3 strafe = zox_getv(character, Velocity3D);
    strafe.z = (fmod(zox_current_time - zox_tst_attacks_time, 4.0) < 2.0 ? 1 : -1) * zox_tst_attacks_strafe;
    zox_setv(character, Velocity3D, strafe);
    zox_tst_attacks_moved = float_max(zox_tst_attacks_moved, float3_length(float3_subtract(zox_getv(character, Position3D), zox_tst_attacks_start)));
    const float health = zox_tst_attacks_player_health(world, character);
    if (zox_tst_attacks_frames % 60 == 0 && zox_valid(npc)) {
        const entity action = zox_has(npc, ActiveAction) ? zox_getv(npc, ActiveAction) : 0;
        const RaycastVoxelData* ray = zox_get(npc, RaycastVoxelData);
        zox_log("[tst] npc combat [%i] behaviour [%i] action [%s] ray hit [%s] at [%.2f] player health [%.1f] of [%.1f]",
            zox_getv(npc, CombatState), zox_getv(npc, Behaviour), zox_get_name(action),
            zox_get_name(ray->chunk), ray->distance, health, zox_tst_attacks_health);
    }
    return health < zox_tst_attacks_health || zox_current_time - zox_tst_attacks_time > zox_tst_attacks_seconds;
}

void zox_tst_npc_attacks_check(ecs* world) {
    const entity character = zox_tst_player_character(world);
    const float health = zox_tst_attacks_player_health(world, character);
    zox_log("[tst] player health [%.1f] from [%.1f] after [%.1fs]", health, zox_tst_attacks_health,
        zox_current_time - zox_tst_attacks_time);
    zox_tst_check(zox_valid(zox_tst_attacks_npc), "the npc is gone");
    zox_log("[tst] the player strafed [%.1f] away", zox_tst_attacks_moved);
    // its shots spent its mana (shoot's SkillResource)
    const entity npc = zox_tst_attacks_npc;
    entity skills[16];
    const uint count = zox_valid(npc) ? zox_get_children_by_id(world, npc, skills, 16, zox_id(Shoot)) : 0;
    const entity mana = count ? get_user_stat_from_realm(world, npc, zox_get_link(world, skills[0], SkillResource)) : 0;
    zox_tst_check(zox_valid(mana), "the npc has no shoot skill, or its resource stat");
    if (zox_valid(mana)) {
        zox_log("[tst] npc shoot's resource [%s] [%.1f] of [%.1f]", zox_get_name(mana), zox_getv(mana, StatValue), zox_getv(mana, StatValueMax));
        zox_tst_check(zox_getv(mana, StatValue) < zox_getv(mana, StatValueMax), "the npc's shots spent no mana");
    }
    zox_tst_check(zox_tst_attacks_moved > 1, "the player didn't strafe, [%.1f]", zox_tst_attacks_moved);
    zox_tst_check(health < zox_tst_attacks_health, "the npc didn't hurt the player in [%.0fs]", zox_tst_attacks_seconds);
}

// tst: region_population, the player's region has its population map, busy at its towns (npcs/sys/population.c)
//  - one cell a chunk column, every cell at least the base, a town's column full
//  - run: bash bsh/tst.sh region_population
byte zox_tst_region_population_ready(ecs* world) {
    return zox_tst_player_spawn_ready(world);
}

void zox_tst_region_population_check(ecs* world) {
    const entity character = zox_tst_player_character(world);
    const entity terrain = zox_get_link(world, character, TerrainLink);
    const int chunk_length = octree_size(zox_getv(terrain, NodeDepth));
    // the region under the player's tunk
    const float3 position = zox_getv(character, Position3D);
    const int3 chunk = real_position_to_chunk_position(position, chunk_length, zox_getv(terrain, BlockScale));
    const TunkLinks* tunks = zox_get(terrain, TunkLinks);
    const entity tunk = int2_hashmap_get(tunks->value, (int2) { chunk.x, chunk.z });
    const entity region = zox_valid(tunk) ? zox_get_link(world, tunk, RegionLink) : 0;
    zox_tst_check(zox_valid(region), "no region under the player");
    if (!zox_valid(region)) {
        return;
    }
    const PopulationMap* map = zox_get(region, PopulationMap);
    const int2 size = population_map_size(zox_getv(region, BlockSize2), chunk_length);
    zox_log("[tst] [metric] region_population [%ix%i] cells, map [%i]", size.x, size.y, map->length);
    zox_tst_check(map->length && map->length == size.x * size.y, "the map is [%i], not [%ix%i]", map->length, size.x, size.y);
    int below = 0;
    for (int i = 0; i < map->length; i++) {
        below += map->value[i] < population_base;
    }
    zox_tst_check(!below, "[%i] cells under the base", below);
    entity towns[zox_children_capacity];
    const uint towns_count = zox_get_children_by_id(world, region, towns, zox_children_capacity, zox_id(Town));
    for (uint i = 0; i < towns_count; i++) {
        const int p = region_population_at(world, region, zox_getv(towns[i], BlockPosition2), chunk_length);
        zox_tst_check(p == population_town, "town [%i]'s column is [%i], not full", i, p);
    }
}

// tst: npc_leash, an npc wandering far from home walks back towards it (ai/sys/wander_system.c)
//  - its home set 30 units away (+x), wandering 6s it walks towards it (at peace, half a unit a second)
//  - run: bash bsh/tst.sh npc_leash
entity zox_tst_leash_npc = 0;
double zox_tst_leash_time = 0;
float zox_tst_leash_start = 0;

static inline float zox_tst_leash_distance(ecs* world, entity npc) {
    const float3 p = zox_getv(npc, Position3D), h = zox_getv(npc, HomePosition);
    return sqrtf((p.x - h.x) * (p.x - h.x) + (p.z - h.z) * (p.z - h.z));
}

byte zox_tst_npc_leash_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_tst_player_character(world);
    if (!zox_tst_leash_npc) {
        const entity realm = zox_get_link(world, character, RealmLink);
        const entity humanoid = zox_tst_realm_humanoid(world, realm);
        if (!zox_valid(humanoid)) {
            zox_tst_check(0, "the realm has no humanoid character prefab");
            return 1;
        }
        float3 position = zox_getv(character, Position3D);
        position.x += 2;
        zox_tst_leash_npc = spawn_character3_npc(world, humanoid, realm, zox_get_link(world, character, TerrainLink), 9, 0, 0,
            position, quaternion_identity, NULL);
        zox_remove(zox_tst_leash_npc, Coward);
        return 0;
    }
    const entity npc = zox_tst_leash_npc;
    zox_tst_waiting = "the npc to wander home";
    if (!zox_tst_leash_time) {
        if (zox_has(npc, PreInitialize) || zox_has(npc, Initialize)) {
            return 0;
        }
        float3 home = zox_getv(npc, Position3D);
        home.x += 30;
        zox_setv(npc, HomePosition, home);
        zox_setv(npc, DefaultBehaviour, zox_behaviour_wander);
        zox_setv(npc, Behaviour, zox_behaviour_wander);
        zox_tst_leash_start = zox_tst_leash_distance(world, npc);
        zox_tst_leash_time = zox_current_time;
        return 0;
    }
    static int logged = 0;
    if ((int) (zox_current_time - zox_tst_leash_time) > logged) {
        logged = (int) (zox_current_time - zox_tst_leash_time);
        const float3 v = zox_getv(npc, Velocity3D);
        zox_log("[tst] leash [%is] home [%.1f] behaviour [%i] move [%i] velocity [%.2f %.2f %.2f]", logged,
            zox_tst_leash_distance(world, npc), zox_getv(npc, Behaviour), zox_getv(npc, MoveForwards), v.x, v.y, v.z);
    }
    return zox_current_time - zox_tst_leash_time > 6;
}

void zox_tst_npc_leash_check(ecs* world) {
    const entity npc = zox_tst_leash_npc;
    zox_tst_check(zox_valid(npc), "the npc is gone");
    if (!zox_valid(npc)) {
        return;
    }
    const float now = zox_tst_leash_distance(world, npc);
    zox_log("[tst] [metric] npc_leash home [%.1f] away, now [%.1f]", zox_tst_leash_start, now);
    // npcs walk about half a unit a second at peace
    zox_tst_check(now < zox_tst_leash_start - 1.5f, "the npc didn't head home, [%.1f] to [%.1f] away", zox_tst_leash_start, now);
    const float3 v = zox_getv(npc, Velocity3D);
    zox_tst_check(v.x > 0, "the npc isn't walking towards its home (+x), velocity x [%.2f]", v.x);
}

// tst: npc_flee_hurt, a fighter hurt badly flees its attacker (ai/sys/behaviour_system.c, npc_flee_health)
//  - an npc 3 away, its health a fifth, hit by the player: it flees (not attacks) and gets further away
//  - run: bash bsh/tst.sh npc_flee_hurt
entity zox_tst_flee_npc = 0;
double zox_tst_flee_time = 0;
float zox_tst_flee_start = 0;

static inline float zox_tst_flee_distance(ecs* world, entity npc) {
    const float3 a = zox_getv(npc, Position3D), b = zox_getv(zox_tst_player_character(world), Position3D);
    return sqrtf((a.x - b.x) * (a.x - b.x) + (a.z - b.z) * (a.z - b.z));
}

byte zox_tst_npc_flee_hurt_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_tst_player_character(world);
    if (!zox_tst_flee_npc) {
        const entity realm = zox_get_link(world, character, RealmLink);
        const entity humanoid = zox_tst_realm_humanoid(world, realm);
        if (!zox_valid(humanoid)) {
            zox_tst_check(0, "the realm has no humanoid character prefab");
            return 1;
        }
        float3 position = zox_getv(character, Position3D);
        position.x += 3;
        zox_tst_flee_npc = spawn_character3_npc(world, humanoid, realm, zox_get_link(world, character, TerrainLink),
            7, 0, 0, position, quaternion_identity, NULL);
        zox_remove(zox_tst_flee_npc, Coward);
        return 0;
    }
    const entity npc = zox_tst_flee_npc;
    if (!zox_tst_flee_time) {
        zox_tst_waiting = "the npc to initialize";
        const entity health = zox_valid(npc) ? zox_get_child_by_id(world, npc, zox_id(StatHealth)) : 0;
        if (!zox_valid(health) || zox_has(npc, PreInitialize) || zox_has(npc, Initialize)) {
            return 0;
        }
        zox_setv(health, StatValue, 0.2f * zox_getv(health, StatValueMax));
        combat_on_hit(world, npc, character);
        zox_tst_flee_time = zox_current_time;
        zox_tst_flee_start = zox_tst_flee_distance(world, npc);
        return 0;
    }
    zox_tst_waiting = "the hurt npc to run";
    return zox_current_time - zox_tst_flee_time > 3;
}

void zox_tst_npc_flee_hurt_check(ecs* world) {
    const entity npc = zox_tst_flee_npc;
    zox_tst_check(zox_valid(npc), "the npc is gone");
    if (!zox_valid(npc)) {
        return;
    }
    const float now = zox_tst_flee_distance(world, npc);
    zox_log("[tst] [metric] npc_flee_hurt behaviour [%i] distance [%.1f] to [%.1f]", zox_getv(npc, Behaviour), zox_tst_flee_start, now);
    zox_tst_check(zox_getv(npc, Behaviour) == zox_behaviour_flee, "the hurt npc's behaviour is [%i], not fleeing", zox_getv(npc, Behaviour));
    zox_tst_check(now > zox_tst_flee_start + 2, "the hurt npc didn't run, [%.1f] to [%.1f] away", zox_tst_flee_start, now);
}

// tst: npc_skill_switch, a humanoid npc picks its skill by its threat's distance (ai/fun/skills.c)
//  - it has punch and shoot (humanoids, actions/sys/character.c); 6 away it shoots, the player beside it it punches
//  - run: bash bsh/tst.sh npc_skill_switch
entity zox_tst_switch_npc = 0;
byte zox_tst_switch_stage = 0;
double zox_tst_switch_time = 0;
byte zox_tst_switch_far = 0;            // shoot picked far
byte zox_tst_switch_near = 0;           // punch picked near

static inline byte zox_tst_switch_action_is(ecs* world, entity npc, entity tag) {
    const entity action = zox_getv(npc, ActiveAction);
    return zox_valid(action) && zox_has_id(action, tag);
}

byte zox_tst_npc_skill_switch_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_tst_player_character(world);
    const entity npc = zox_tst_switch_npc;
    if (zox_tst_switch_stage == 0) {
        const entity realm = zox_get_link(world, character, RealmLink);
        const entity humanoid = zox_tst_realm_humanoid(world, realm);
        if (!zox_valid(humanoid)) {
            zox_tst_check(0, "the realm has no humanoid character prefab");
            return 1;
        }
        float3 position = zox_getv(character, Position3D);
        position.x += 6;
        zox_tst_switch_npc = spawn_character3_npc(world, humanoid, realm, zox_get_link(world, character, TerrainLink),
            11, 0, 0, position, quaternion_identity, NULL);
        zox_remove(zox_tst_switch_npc, Coward);
        zox_tst_switch_stage = 1;
        return 0;
    }
    if (zox_tst_switch_stage == 1) {
        zox_tst_waiting = "the npc to initialize";
        if (!zox_valid(npc) || zox_has(npc, PreInitialize) || zox_has(npc, Initialize) ||
            !zox_valid(zox_get_child_by_id(world, npc, zox_id(Skill)))) {
            return 0;
        }
        combat_on_hit(world, npc, character);
        zox_tst_switch_time = zox_current_time;
        zox_tst_switch_stage = 2;
        return 0;
    }
    if (zox_tst_switch_stage == 2) {
        zox_tst_waiting = "the npc to pick shoot, 6 away";
        zox_tst_switch_far |= zox_tst_switch_action_is(world, npc, zox_id(Shoot));
        if (zox_tst_switch_far || zox_current_time - zox_tst_switch_time > 4) {
            // the player beside it
            float3 p = zox_getv(npc, Position3D);
            p.x -= 1;
            zox_setv(character, Position3D, p);
            zox_tst_switch_time = zox_current_time;
            zox_tst_switch_stage = 3;
        }
        return 0;
    }
    zox_tst_waiting = "the npc to pick punch, beside it";
    zox_tst_switch_near |= zox_tst_switch_action_is(world, npc, zox_id(Melee));
    return zox_tst_switch_near || zox_current_time - zox_tst_switch_time > 4;
}

void zox_tst_npc_skill_switch_check(ecs* world) {
    zox_log("[tst] npc_skill_switch far shoot [%i] near punch [%i]", zox_tst_switch_far, zox_tst_switch_near);
    zox_tst_check(zox_tst_switch_far, "the npc didn't pick shoot 6 away");
    zox_tst_check(zox_tst_switch_near, "the npc didn't pick punch beside the player");
}

// tst: shoot_burn, a shot burns who it hits (skills/pre/burn.c): a short dot, its skill's colour rising off them
//  - a humanoid 5 away shoots the player; once hit, it's gone (only the burn hurts them after)
//  - the player has a burn (a Dot) with its streamer, their health drops while it burns, it's gone in its time (5s)
//  - tst/shoot_burn/burn.ppm, the player burning, from the side
//  - run: bash bsh/tst.sh shoot_burn
entity zox_tst_burn_npc = 0;
byte zox_tst_burn_stage = 0;
double zox_tst_burn_time = 0;
float zox_tst_burn_health = 0;          // as the shot hit
float zox_tst_burn_health_after = 0;    // as the burn ended
byte zox_tst_burn_seen = 0;             // a burn with its streamer on the player
byte zox_tst_burn_gone = 0;
byte zox_tst_burn_trail = 0;            // a shot with its stream

static inline entity zox_tst_burn_on(ecs* world, entity character) {
    entity burns[8];
    return zox_get_children_by_id(world, character, burns, 8, zox_id(Dot)) ? burns[0] : 0;
}

byte zox_tst_shoot_burn_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_tst_player_character(world);
    if (zox_tst_burn_stage == 0) {
        const entity realm = zox_get_link(world, character, RealmLink);
        const entity humanoid = zox_tst_realm_humanoid(world, realm);
        if (!zox_valid(humanoid)) {
            zox_tst_check(0, "the realm has no humanoid character prefab");
            return 1;
        }
        float3 position = zox_getv(character, Position3D);
        position.x += 5;
        zox_tst_burn_npc = spawn_character3_npc(world, humanoid, realm, zox_get_link(world, character, TerrainLink),
            31, 0, 0, position, quaternion_identity, NULL);
        zox_remove(zox_tst_burn_npc, Coward);
        zox_tst_burn_stage = 1;
        return 0;
    }
    const entity npc = zox_tst_burn_npc;
    if (zox_tst_burn_stage == 1) {
        zox_tst_waiting = "the npc to initialize";
        if (!zox_valid(npc) || zox_has(npc, PreInitialize) || zox_has(npc, Initialize) ||
            !zox_valid(zox_get_child_by_id(world, npc, zox_id(Skill)))) {
            return 0;
        }
        combat_on_hit(world, npc, character);
        zox_tst_burn_time = zox_current_time;
        zox_tst_burn_stage = 2;
        return 0;
    }
    if (zox_tst_burn_stage == 2) {
        zox_tst_waiting = "a shot to burn the player";
        zox_tst_burn_trail |= zox_count_types(ParticleStreamer) > 0;       // only shots stream, before a burn
        const entity burn = zox_tst_burn_on(world, character);
        if (zox_valid(burn)) {
            // its flames: a streamer (its prefab's copy) in its colour, its skill's
            const color c = zox_getv(burn, Color), s = zox_getv(zox_get_link(world, burn, Skill), Color);
            zox_tst_burn_seen = zox_valid(get_particle3_streamer(world, burn)) && c.r == s.r && c.g == s.g && c.b == s.b;
            zox_tst_burn_health = zox_tst_attacks_player_health(world, character);
            zox_delete(npc);
            zox_tst_burn_time = zox_current_time;
            zox_tst_burn_stage = 3;
        }
        return zox_current_time - zox_tst_burn_time > 15;
    }
    zox_tst_waiting = "the burn to end";
    const entity camera = zox_get_link(world, dbg_player, CameraLink);
    if (zox_valid(camera)) {
        const float3 at = zox_getv(character, Position3D);
        const float3 eye = float3_add(at, (float3) { 2.5f, 0.5f, 0 });
        if (zox_get_parent(world, camera)) {
            zox_set_parent(world, camera, 0);
        }
        zox_setv(camera, Position3D, eye);
        zox_setv(camera, Rotation3D, camera_look_at(eye, at));
    }
    if (zox_current_time - zox_tst_burn_time > 0.8 && zox_tst_burn_stage == 3 && !zox_screenshot_request[0]) {
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("shoot_burn/burn.ppm"));
        zox_tst_burn_stage = 4;
    }
    if (!zox_valid(zox_tst_burn_on(world, character)) && zox_tst_burn_stage == 4 && !zox_screenshot_request[0]) {
        zox_tst_burn_gone = 1;
        zox_tst_burn_health_after = zox_tst_attacks_player_health(world, character);
        return 1;
    }
    return zox_current_time - zox_tst_burn_time > 8;
}

void zox_tst_shoot_burn_check(ecs* world) {
    zox_log("[tst] shoot_burn trail [%i] burn [%i] health [%.2f] to [%.2f] gone [%i] after [%.1fs]", zox_tst_burn_trail,
        zox_tst_burn_seen, zox_tst_burn_health, zox_tst_burn_health_after, zox_tst_burn_gone, zox_current_time - zox_tst_burn_time);
    zox_tst_check(zox_tst_burn_trail, "no shot had a particle trail");
    zox_tst_check(zox_tst_burn_seen, "the player wasn't burnt with particles off them");
    zox_tst_check(zox_tst_burn_gone && zox_current_time - zox_tst_burn_time < 6, "the burn didn't end in its 5s");
    zox_tst_check(zox_tst_burn_health_after < zox_tst_burn_health - 1.5f, "the burn didn't hurt, [%.2f] to [%.2f]",
        zox_tst_burn_health, zox_tst_burn_health_after);
}

void zox_tsts_npc_attacks(ecs* world) {
    zox_tst_add("npc_attacks", zox_tst_npc_attacks_ready, zox_tst_npc_attacks_check, 120);
    zox_tst_set_setup("npc_attacks", zox_tst_level_setup, 0);
    zox_tst_add("region_population", zox_tst_region_population_ready, zox_tst_region_population_check, 120);
    zox_tst_add("npc_leash", zox_tst_npc_leash_ready, zox_tst_npc_leash_check, 120);
    zox_tst_set_setup("npc_leash", zox_tst_level_setup, 0);
    zox_tst_add("npc_flee_hurt", zox_tst_npc_flee_hurt_ready, zox_tst_npc_flee_hurt_check, 120);
    zox_tst_set_setup("npc_flee_hurt", zox_tst_level_setup, 0);
    zox_tst_add("npc_skill_switch", zox_tst_npc_skill_switch_ready, zox_tst_npc_skill_switch_check, 120);
    zox_tst_set_setup("npc_skill_switch", zox_tst_level_setup, 0);
    zox_tst_add("shoot_burn", zox_tst_shoot_burn_ready, zox_tst_shoot_burn_check, 120);
    zox_tst_set_setup("shoot_burn", zox_tst_level_setup, 0);
}
