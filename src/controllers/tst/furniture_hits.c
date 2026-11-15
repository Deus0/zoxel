// tst: furniture_hits, the ray and collisions find all of a bed and a door (fillers, items/fun/multi_block.c)
//  - places a bed and a door in air with air above
//  - rays straight down onto the bed's filler, the bed's base and the door's top stop there on the bed's or door's own
//    voxels (a filler's into its base's model, raycast_voxel_node)
//  - an npc dropped onto the bed's filler and one onto the door rest on top, not falling through
//  - run: bash bsh/tst.sh furniture_hits
byte zox_tst_fhits_stage = 0;
int zox_tst_fhits_frames = 0;
double zox_tst_fhits_time = 0;
int3 zox_tst_fhits_bed, zox_tst_fhits_door;
entity zox_tst_fhits_npcs[2];

// a ray from origin straight down, its result
static inline RaycastVoxelData zox_tst_fhits_ray(ecs* world, entity terrain, entity realm, float3 origin, float range) {
    RaycastVoxelData data = { 0 };
    CharacterRaycast character_raycast = { 0 };
    const byte depth = zox_getv(terrain, NodeDepth);
    data.result = raycast_voxel_node(world, 0, zox_get(realm, BlockLinks), zox_get(terrain, ChunkLinks), int3_zero, float3_zero,
        depth, int3_single(octree_size(depth)), 0, origin, (float3) { 0, -1, 0 }, int3_zero, zox_getv(terrain, BlockScale),
        range, &data, &character_raycast);
    return data;
}

// a block's middle, in the world
static inline float3 zox_tst_fhits_middle(int3 block, float scale) {
    return (float3) { (block.x + 0.5f) * scale, (block.y + 0.5f) * scale, (block.z + 0.5f) * scale };
}

byte zox_tst_furniture_hits_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    const entity terrain = zox_get_link(world, character, TerrainLink);
    const entity realm = zox_get_link(world, character, RealmLink);
    const float scale = zox_getv(terrain, BlockScale);
    zox_tst_fhits_frames++;
    zox_tst_progress(zox_tst_fhits_stage * 1000 + zox_tst_fhits_frames);
    if (zox_tst_fhits_stage == 0) {
        zox_tst_waiting = "air for a bed and a door, with air above";
        const byte depth = zox_getv(terrain, NodeDepth);
        const int length = octree_size(depth);
        const int3 chunk = real_position_to_chunk_position(zox_getv(character, Position3D), length, scale);
        const int3 origin = { chunk.x * length, chunk.y * length, chunk.z * length };
        // in the player's chunk or the one above
        for (int y = 0; y < length * 2 - 8; y++) {
            const int3 bed = { origin.x + 3, origin.y + y, origin.z + 8 };
            const int3 door = { origin.x + 9, origin.y + y, origin.z + 8 };
            int solid = 0;
            for (int h = 0; h < 8; h++) {
                solid += zox_tst_ffill_at(world, terrain, bed, 0, h, 0) != 0 || zox_tst_ffill_at(world, terrain, bed, 1, h, 0) != 0 ||
                    zox_tst_ffill_at(world, terrain, door, 0, h, 0) != 0;
            }
            if (solid) {
                continue;
            }
            const byte bed_voxel = zox_getv(zox_tst_fplace_block(world, realm, zox_id(BlockBed)), BlockIndex);
            const byte door_voxel = zox_getv(zox_tst_fplace_block(world, realm, zox_id(BlockDoor)), BlockIndex);
            zox_tst_check(multi_block_place(world, terrain, realm, bed, bed_voxel, 0), "the bed wasn't placed");
            zox_tst_check(multi_block_place(world, terrain, realm, door, door_voxel, 0), "the door wasn't placed");
            zox_tst_fhits_bed = bed;
            zox_tst_fhits_door = door;
            zox_tst_fhits_stage = 1;
            zox_tst_fhits_frames = 0;
            return 0;
        }
        return zox_tst_fhits_frames > 600;
    }
    if (zox_tst_fhits_stage == 1) {
        zox_tst_waiting = "the bed's block vox and its mesh";
        // (lod meshes build when first shown, slowly in software: up to a minute)
        byte3 local;
        const entity chunk = terrain_chunk_local(world, terrain, zox_tst_fhits_bed, &local);
        const entity vode = chunk ? zox_tst_fplace_vode(world, chunk, local) : 0;
        const entity mesh = zox_valid(vode) ? zox_get_link(world, vode, InstanceLink) : 0;
        const byte ready = zox_valid(mesh) && zox_has(mesh, MeshVertices) && zox_get(mesh, MeshVertices)->length;
        if (zox_tst_fhits_frames < 10 || (!ready && zox_tst_fhits_frames < 3600)) {
            return 0;
        }
        zox_tst_check(ready, "the bed's block vox never had a mesh");
        // rays from 3 blocks above each part's top, straight down
        const int3 b = zox_tst_fhits_bed, d = zox_tst_fhits_door;
        const int3 parts[3] = { { b.x + 1, b.y, b.z }, b, { d.x, d.y + 1, d.z } };
        const char* names[3] = { "the bed's filler", "the bed's base", "the door's top" };
        const byte filler = realm_filler_voxel(world, realm);
        for (byte i = 0; i < 3; i++) {
            float3 from = zox_tst_fhits_middle(parts[i], scale);
            from.y += 3 * scale;
            const RaycastVoxelData ray = zox_tst_fhits_ray(world, terrain, realm, from, 6 * scale);
            const float stop = from.y - ray.distance;
            const byte voxel = ray.hit_block ? zox_getv(ray.hit_block, BlockIndex) : 0;
            zox_log("[tst] ray onto %s: result [%i] block [%s] stopped at [%.2f] blocks, its cell [%i to %i]", names[i], ray.result,
                ray.hit_block ? zox_getn(ray.hit_block) : "none", stop / scale, parts[i].y, parts[i].y + 1);
            zox_tst_check(ray.result != rayhit_none, "the ray onto %s hit nothing", names[i]);
            // the bed's or door's own voxels, a filler too (into its base's model)
            zox_tst_check(voxel && voxel != filler, "the ray onto %s hit [%s]", names[i], ray.hit_block ? zox_getn(ray.hit_block) : "none");
            zox_tst_check(stop >= parts[i].y * scale - 0.01f && stop <= (parts[i].y + 1) * scale + 0.01f,
                "the ray onto %s stopped at [%.2f] blocks, not in its cell [%i to %i]", names[i], stop / scale, parts[i].y, parts[i].y + 1);
        }
        // drop an npc onto the bed's filler and one onto the door
        const entity humanoid = zox_tst_realm_humanoid(world, realm);
        for (byte i = 0; i < 2; i++) {
            float3 at = zox_tst_fhits_middle(i ? (int3) { d.x, d.y + 1, d.z } : (int3) { b.x + 1, b.y, b.z }, scale);
            at.y += 3 * scale;
            zox_tst_fhits_npcs[i] = spawn_character3_npc(world, humanoid, realm, terrain, 3 + i, 0, 0, at, quaternion_identity, NULL);
        }
        zox_tst_fhits_stage = 2;
        zox_tst_fhits_frames = 0;
        zox_tst_fhits_time = zox_current_time;
        return 0;
    }
    zox_tst_waiting = "the npcs to land";
    // held still where they land (they'd wander off)
    for (byte i = 0; i < 2; i++) {
        if (zox_valid(zox_tst_fhits_npcs[i]) && zox_has(zox_tst_fhits_npcs[i], Behaviour)) {
            zox_setv(zox_tst_fhits_npcs[i], Behaviour, zox_behaviour_idle);
        }
    }
    // landed: both fallen from where they spawned and still; they're held still their first 8 physics frames
    //  (physics3d_disable_system), and frames here can take a second while the beds' meshes first build (software)
    byte landed = 1;
    for (byte i = 0; i < 2; i++) {
        const entity npc = zox_tst_fhits_npcs[i];
        if (!zox_valid(npc) || !zox_has(npc, Velocity3D)) {
            continue;
        }
        const float top = i ? (zox_tst_fhits_door.y + 2) * scale : (zox_tst_fhits_bed.y + 1) * scale;
        const float feet = zox_getv(npc, Position3D).y - zox_getv(npc, Bounds3D).y;
        landed &= feet < top + 1.5f * scale && fabsf(zox_getv(npc, Velocity3D).y) < 0.01f;
    }
    const double waited = zox_current_time - zox_tst_fhits_time;
    return (waited > 2 && landed) || waited > 15;
}

void zox_tst_furniture_hits_check(ecs* world) {
    zox_tst_check(zox_tst_fhits_stage == 2, "it stopped at stage [%i]", zox_tst_fhits_stage);
    if (zox_tst_fhits_stage != 2) {
        return;
    }
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    const float scale = zox_getv(zox_get_link(world, character, TerrainLink), BlockScale);
    const float tops[2] = { (zox_tst_fhits_bed.y + 1) * scale, (zox_tst_fhits_door.y + 2) * scale };
    const char* names[2] = { "the bed's filler", "the door" };
    for (byte i = 0; i < 2; i++) {
        const entity npc = zox_tst_fhits_npcs[i];
        zox_tst_check(zox_valid(npc), "the npc dropped onto %s is gone", names[i]);
        if (!zox_valid(npc)) {
            continue;
        }
        const float feet = zox_getv(npc, Position3D).y - zox_getv(npc, Bounds3D).y;
        zox_log("[tst] npc on %s: feet at [%.2f] blocks, its top [%.2f]", names[i], feet / scale, tops[i] / scale);
        zox_tst_check(fabsf(feet - tops[i]) < 0.2f * scale, "the npc dropped onto %s stands at [%.2f] blocks, not its top [%.2f]",
            names[i], feet / scale, tops[i] / scale);
    }
}
