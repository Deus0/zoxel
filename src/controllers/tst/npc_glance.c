// tst: npc_glance, a humanoid npc's head turns to the player walking by (ai/sys/glance.c, bones/sys/head_look.c)
//  - spawns one 2 in front of the player facing them, across a chunk's edge (it sees into neighbour chunks), its head looks at the player and faces them
//  - the player moves away, its head looks ahead again
//  - run: bash bsh/tst.sh npc_glance
int zox_tst_glance_stage = 0;
int zox_tst_glance_frames = 0;
entity zox_tst_glance_npc = 0;
entity zox_tst_glance_seen = 0;     // its head's LookTarget, with the player near
float zox_tst_glance_facing = -2;   // how directly its head faced the player, 1 straight at them
entity zox_tst_glance_after = 0;    // its head's LookTarget, the player gone

static inline entity zox_tst_glance_head(ecs* world, entity e) {
    const entity skeleton = get_skeleton(world, e);
    return zox_valid(skeleton) ? zox_get_link(world, skeleton, HeadBoneLink) : 0;
}

byte zox_tst_npc_glance_ready(ecs* world) {
    const entity player = dbg_player;
    const entity character = zox_valid(player) ? zox_get_link(world, player, CharacterLink) : 0;
    if (!zox_valid(character)) {
        zox_tst_waiting = "the player's character";
        return 0;
    }
    zox_tst_glance_frames++;
    zox_tst_progress(zox_tst_glance_stage);
    if (zox_tst_glance_stage == 0) {
        zox_tst_waiting = "playing";
        if (zox_getv(player, PlayerState) != zox_player_state_playing || zox_tst_glance_frames < 10) {
            return 0;
        }
        const entity realm = zox_get_link(world, character, RealmLink);
        const entity terrain = zox_get_link(world, character, TerrainLink);
        const entity humanoid = zox_tst_realm_humanoid(world, realm);
        if (!zox_valid(humanoid)) {
            zox_tst_check(0, "the realm has no humanoid character prefab");
            return 1;
        }
        // across a chunk's edge: the player just inside its chunk's low z edge, the npc 2 back in the next chunk,
        //  facing +z (identity) at the player
        const float chunk_length = octree_size(zox_getv(terrain, NodeDepth)) * zox_getv(terrain, BlockScale);
        float3 position = zox_getv(character, Position3D);
        position.z = floorf(position.z / chunk_length) * chunk_length + 0.5f;
        zox_setv(character, Position3D, position);
        zox_setv(character, Velocity3D, float3_zero);
        position.z -= 2;
        zox_tst_glance_npc = spawn_character3_npc(world, humanoid, realm, terrain, 1, 0, 0, position, quaternion_identity, NULL);
        zox_tst_glance_stage = 1;
        zox_tst_glance_frames = 0;
        return 0;
    }
    if (zox_tst_glance_stage == 1) {
        zox_tst_waiting = "the npc to look at the player";
        const entity head = zox_tst_glance_head(world, zox_tst_glance_npc);
        // its best facing over two seconds once it looks (it may wander and turn its body)
        if (zox_valid(head) && zox_getv(head, LookTarget) == character) {
            if (!zox_tst_glance_seen) {
                zox_log("[tst] npc in chunk [%s], the player in [%s]", zox_getn(zox_get_link(world, zox_tst_glance_npc, ChunkLink)), zox_getn(zox_get_link(world, character, ChunkLink)));
                zox_tst_glance_frames = 0;
            }
            zox_tst_glance_seen = character;
            const float3 forward = float4_rotate_float3(zox_getv(head, Rotation3D), float3_forward);
            const float3 to = float3_normalize(float3_sub(look_point(world, character), zox_getv(head, Position3D)));
            zox_tst_glance_facing = float_max(zox_tst_glance_facing, float3_dot(forward, to));
        }
        if ((zox_tst_glance_seen && zox_tst_glance_frames > 120) || zox_tst_glance_frames > 600) {
            // the player walks away
            float3 position = zox_getv(character, Position3D);
            position.x += 24;
            zox_setv(character, Position3D, position);
            zox_tst_glance_stage = 2;
            zox_tst_glance_frames = 0;
        }
        return 0;
    }
    zox_tst_waiting = "the npc to look ahead";
    const entity head = zox_tst_glance_head(world, zox_tst_glance_npc);
    zox_tst_glance_after = zox_valid(head) ? zox_getv(head, LookTarget) : 0;
    return !zox_tst_glance_after || zox_tst_glance_frames > 300;
}

void zox_tst_npc_glance_check(ecs* world) {
    (void) world;
    zox_log("[tst] [metric] npc_glance its head faced the player [%.2f] (1 straight at them)", zox_tst_glance_facing);
    zox_tst_check(zox_tst_glance_seen, "the npc's head didn't look at the player 2 in front of it");
    zox_tst_check(zox_tst_glance_facing > 0.8f, "the npc's head faced the player [%.2f], not at them", zox_tst_glance_facing);
    zox_tst_check(!zox_tst_glance_after, "the npc's head still looks at the player after they left");
}
