// tst: npc_attacks, an npc the player hits fights back and hurts the player
//  - spawns a humanoid npc beside the player (not a coward), hits it as the player would (combat_on_hit)
//  - it goes into battle, attacks (Behaviour), triggers its skill, and the player's health drops
//  - logs where the chain stops: battle, behaviour, skill, its ray's hit, the player's health
//  - run: bash bsh/tst.sh npc_attacks
byte zox_tst_player_spawn_ready(ecs* world);   // in _.c, after this is included
byte zox_tst_attacks_stage = 0;
int zox_tst_attacks_frames = 0;
double zox_tst_attacks_time = 0;
entity zox_tst_attacks_npc = 0;
float zox_tst_attacks_health = 0;
#define zox_tst_attacks_seconds 15.0

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
        position.x += 1.5f;
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
        zox_tst_attacks_stage = 2;
        return 0;
    }
    zox_tst_waiting = "the npc to hurt the player";
    const entity npc = zox_tst_attacks_npc;
    const float health = zox_tst_attacks_player_health(world, character);
    if (zox_tst_attacks_frames % 60 == 0 && zox_valid(npc)) {
        const entity action = zox_has(npc, ActiveAction) ? zox_getv(npc, ActiveAction) : 0;
        zox_geter(npc, RaycastVoxelData, ray);
        zox_log("[tst] npc combat [%i] behaviour [%i] action [%s] ray hit [%s] at [%.2f] player health [%.1f] of [%.1f]",
            zox_getv(npc, CombatState), zox_getv(npc, Behaviour), zox_get_name(action),
            zox_get_name(ray->chunk), ray->distance, health, zox_tst_attacks_health);
    }
    return health < zox_tst_attacks_health || zox_current_time - zox_tst_attacks_time > zox_tst_attacks_seconds;
}

void zox_tst_npc_attacks_check(ecs* world) {
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    const float health = zox_tst_attacks_player_health(world, character);
    zox_log("[tst] player health [%.1f] from [%.1f] after [%.1fs]", health, zox_tst_attacks_health,
        zox_current_time - zox_tst_attacks_time);
    zox_tst_check(zox_valid(zox_tst_attacks_npc), "the npc is gone");
    zox_tst_check(health < zox_tst_attacks_health, "the npc didn't hurt the player in [%.0fs]", zox_tst_attacks_seconds);
}
