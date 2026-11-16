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
int3 zox_tst_fhits_doors[4];    // a door turned each way (0 is zox_tst_fhits_door)
entity zox_tst_fhits_npcs[2];

// a ray from origin straight down, its result
static inline RaycastVoxelData zox_tst_fhits_ray_dir(ecs* world, entity terrain, entity realm, float3 origin, float3 normal, float range) {
    RaycastVoxelData data = { 0 };
    CharacterRaycast character_raycast = { 0 };
    const byte depth = zox_getv(terrain, NodeDepth);
    data.result = raycast_voxel_node(world, 0, zox_get(realm, BlockLinks), zox_get(terrain, ChunkLinks), int3_zero, float3_zero,
        depth, int3_single(octree_size(depth)), 0, origin, normal, int3_zero, zox_getv(terrain, BlockScale), range, &data, &character_raycast);
    return data;
}

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
    const entity character = zox_tst_player_character(world);
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
                for (int k = 1; k < 4; k++) {
                    for (int s = -1; s <= 1; s++) {
                        solid += zox_tst_ffill_at(world, terrain, door, s, h, k * 3) != 0 || zox_tst_ffill_at(world, terrain, door, 0, h, k * 3 + s) != 0;
                    }
                }
            }
            if (solid) {
                continue;
            }
            const byte bed_voxel = zox_getv(zox_tst_fplace_block(world, realm, zox_id(BlockBed)), BlockIndex);
            const byte door_voxel = zox_getv(zox_tst_fplace_block(world, realm, zox_id(BlockDoor)), BlockIndex);
            zox_tst_check(zox_tst_place_supported(world, terrain, realm, bed, bed_voxel, 0), "the bed wasn't placed");
            zox_tst_check(zox_tst_place_supported(world, terrain, realm, door, door_voxel, 0), "the door wasn't placed");
            zox_tst_fhits_doors[0] = door;
            for (byte k = 1; k < 4; k++) {
                zox_tst_fhits_doors[k] = (int3) { door.x, door.y, door.z + k * 3 };
                zox_tst_check(zox_tst_place_supported(world, terrain, realm, zox_tst_fhits_doors[k], door_voxel, k), "door [%i] wasn't placed", k);
            }
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
        // the door's top half from the side, both ways along x and z (looking at a door)
        const byte door_voxel = zox_getv(zox_tst_fplace_block(world, realm, zox_id(BlockDoor)), BlockIndex);
        const float3 sides[4] = { { 1, 0, 0 }, { -1, 0, 0 }, { 0, 0, 1 }, { 0, 0, -1 } };
        for (byte k = 0; k < 4; k++)
        for (byte j = 0; j < 4; j++) {
            const int3 d = zox_tst_fhits_doors[k];
            const float3 middle = zox_tst_fhits_middle((int3) { d.x, d.y + 1, d.z }, scale);
            // from the next cell over (the doors are 3 apart), into it
            const float3 from = float3_sub(middle, float3_scale(sides[j], 1.4f * scale));
            const RaycastVoxelData ray = zox_tst_fhits_ray_dir(world, terrain, realm, from, sides[j], 2 * scale);
            const float3 hit = float3_add(from, float3_scale(sides[j], ray.distance));
            const int3 cell = real_position_to_block_position(float3_add(hit, float3_scale(sides[j], 0.001f)), scale);
            zox_log("[tst] ray at door [%i]'s top from the side [%.0f %.0f]: result [%i] block [%s] cell [%i %i %i], its top [%i %i %i]", k, sides[j].x,
                sides[j].z, ray.result, ray.hit_block ? zox_getn(ray.hit_block) : "none", cell.x, cell.y, cell.z, d.x, d.y + 1, d.z);
            zox_tst_check(ray.hit_block && zox_getv(ray.hit_block, BlockIndex) == door_voxel && int3_equals(cell, (int3) { d.x, d.y + 1, d.z }),
                "a ray at door [%i]'s top from the side [%.0f %.0f] hit [%s] in [%i %i %i], not its top", k, sides[j].x, sides[j].z,
                ray.hit_block ? zox_getn(ray.hit_block) : "none", cell.x, cell.y, cell.z);
        }
        // like a player looking at a door: from eye height a few blocks off, at points across its top cell's face
        int through = 0, aimed = 0;
        for (byte k = 0; k < 4; k++)
        for (byte j = 0; j < 4; j++)
        for (int u = -1; u <= 1; u++)
        for (int v = -1; v <= 1; v++) {
            const int3 dd = zox_tst_fhits_doors[k];
            const float3 normal_side = sides[j];
            // at its face (a door's thin, along its panel the off middle rays pass it, as they should)
            const int3 facing = block_direction_offset(k, (int3) { 0, 0, 1 });
            if (fabsf(normal_side.x * facing.x + normal_side.z * facing.z) < 0.5f) {
                continue;
            }
            const float3 across = { normal_side.z, 0, normal_side.x };
            float3 target = zox_tst_fhits_middle((int3) { dd.x, dd.y + 1, dd.z }, scale);
            target = float3_add(target, float3_scale(across, u * 0.3f * scale));
            target.y += v * 0.3f * scale;
            const float3 eye = float3_add(float3_sub(target, float3_scale(normal_side, 1.3f * scale)), (float3) { 0, 0.4f * scale, 0 });
            const float3 to = float3_normalize(float3_sub(target, eye));
            const RaycastVoxelData ray = zox_tst_fhits_ray_dir(world, terrain, realm, eye, to, 4 * scale);
            const float3 hit = float3_add(eye, float3_scale(to, ray.distance));
            const int3 cell = real_position_to_block_position(float3_add(hit, float3_scale(to, 0.001f)), scale);
            aimed++;
            const byte ok = ray.hit_block && zox_getv(ray.hit_block, BlockIndex) == door_voxel && int3_equals(cell, (int3) { dd.x, dd.y + 1, dd.z });
            if (!ok) {
                through++;
                if (through <= 6) {
                    zox_log("[tst] eye ray at door [%i] side [%.0f %.0f] point [%i %i]: result [%i] block [%s] cell [%i %i %i] (its top [%i %i %i])", k,
                        normal_side.x, normal_side.z, u, v, ray.result, ray.hit_block ? zox_getn(ray.hit_block) : "none", cell.x, cell.y, cell.z, dd.x, dd.y + 1, dd.z);
                }
            }
        }
        zox_log("[tst] [metric] eye rays at doors' faces' tops: [%i] of [%i] missed", through, aimed);
        zox_tst_check(!through, "[%i] of [%i] eye rays at doors' faces' tops didn't hit their top cell", through, aimed);
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
    const entity character = zox_tst_player_character(world);
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

void zox_tsts_furniture_hits(ecs* world) {
    zox_tst_add("furniture_hits", zox_tst_furniture_hits_ready, zox_tst_furniture_hits_check, 120);
}
