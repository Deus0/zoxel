// tst: furniture_place, a bed and a door placed in the world are bigger than a block (doc/plans/multi_block_vox.md)
//  - places a flower, a bed and a door in the air of the player's chunk, 4 voxels apart
//  - each spawns one block vox at its voxel; the bed's and door's sit half a block further along every axis than a
//      one block model would (their cube's corner at the base voxel's), and draw a mesh two blocks long or tall
//      starting at the base voxel's corner, at a block's detail (their render depth a depth past the chunk's)
//  - run: bash bsh/tst.sh furniture_place
byte zox_tst_fplace_stage = 0;
double zox_tst_fplace_time = 0;
entity zox_tst_fplace_chunk = 0;
byte3 zox_tst_fplace_at;

static inline entity zox_tst_fplace_vode(ecs* world, entity chunk, byte3 position) {
    const VoxelNode* node = get_VoxelNode(zox_get(chunk, VoxelNode), zox_getv(chunk, NodeDepth), position);
    return node && is_linked_VoxelNode(node) ? get_entity_VoxelNode(node) : 0;
}

// the realm's first block with a tag (biome blocks aren't the realm's children, they're in its BlockLinks)
static inline entity zox_tst_fplace_block(ecs* world, entity realm, entity tag) {
    const BlockLinks* blocks = zox_get(realm, BlockLinks);
    for (int i = 0; i < blocks->length; i++) {
        if (zox_valid(blocks->value[i]) && ecs_has_id(world, blocks->value[i], tag)) {
            return blocks->value[i];
        }
    }
    return 0;
}

byte zox_tst_furniture_place_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_tst_player_character(world);
    const entity terrain = zox_get_link(world, character, TerrainLink);
    const entity realm = zox_get_link(world, character, RealmLink);
    zox_tst_progress(zox_tst_fplace_stage);
    if (zox_tst_fplace_stage == 0) {
        zox_tst_waiting = "air in the player's chunk";
        const byte depth = zox_getv(terrain, NodeDepth);
        const int length = octree_size(depth);
        int3 chunk_position = real_position_to_chunk_position(zox_getv(character, Position3D), length, zox_getv(terrain, BlockScale));
        // three voxels of air with air over them (x 2, 6, 10), in the player's chunk or the one under it, whichever
        //  has spawned its block voxes
        entity chunk = 0;
        int y = -1;
        for (byte below = 0; below < 2 && y < 0; below++, chunk_position.y--) {
            chunk = int3_hashmap_get(zox_get(terrain, ChunkLinks)->value, chunk_position);
            if (!zox_valid(chunk) || !zox_has(chunk, VoxelNodeQueue) || !zox_has(chunk, BlocksSpawned)) {
                continue;
            }
            const VoxelNode* voxels = zox_get(chunk, VoxelNode);
            for (int j = 1; j < length - 2 && y < 0; j++) {
                int solid = 0;
                for (int x = 2; x <= 11; x++) for (int h = 0; h < 2; h++) {
                    solid += getv_VoxelNode(voxels, depth, (byte3) { x, j + h, 8 }) != 0;
                }
                y = solid ? -1 : j;
            }
        }
        if (y < 0) {
            return 0;
        }
        zox_tst_fplace_at = (byte3) { 2, y, 8 };
        VoxelNodeQueue* queue = zox_getm(chunk, VoxelNodeQueue);
        const entity blocks[3] = { zox_tst_fplace_block(world, realm, zox_id(BlockFlower)), zox_tst_fplace_block(world, realm, zox_id(BlockBed)),
            zox_tst_fplace_block(world, realm, zox_id(BlockDoor)) };
        for (byte i = 0; i < 3; i++) {
            zox_tst_check(zox_valid(blocks[i]), "the realm has no %s block", i == 0 ? "flower" : i == 1 ? "bed" : "door");
            if (!zox_valid(blocks[i])) {
                return 1;
            }
            a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = zox_getv(blocks[i], BlockIndex), .position = (byte3) { 2 + i * 4, y, 8 } });
        }
        zox_tst_fplace_chunk = chunk;
        zox_tst_fplace_stage = 1;
        zox_tst_fplace_time = zox_current_time;
        return 0;
    }
    zox_tst_waiting = "the bed's and door's block voxes and their meshes";
    // (their lod meshes build when first shown, slowly in software: progress by the second, up to a minute)
    zox_tst_progress(1000 + (int) (zox_current_time - zox_tst_fplace_time));
    if (zox_current_time - zox_tst_fplace_time < 1) {
        return 0;
    }
    for (byte i = 0; i < 3; i++) {
        const byte3 at = { zox_tst_fplace_at.x + i * 4, zox_tst_fplace_at.y, zox_tst_fplace_at.z };
        const entity vode = zox_tst_fplace_vode(world, zox_tst_fplace_chunk, at);
        const entity mesh = zox_valid(vode) ? zox_get_link(world, vode, InstanceLink) : 0;
        if (!zox_valid(mesh) || !zox_has(mesh, MeshVertices) || !zox_get(mesh, MeshVertices)->length) {
            if (zox_current_time - zox_tst_fplace_time > 60) {
                zox_log("[tst] model [%i]: voxel [%i] block vox [%s] render depth [%i] mesh [%s] vertices [%i]", i,
                    getv_VoxelNode(zox_get(zox_tst_fplace_chunk, VoxelNode), zox_getv(zox_tst_fplace_chunk, NodeDepth), at),
                    zox_valid(vode) ? zox_getn(vode) : "none", zox_valid(vode) ? zox_getv(vode, RenderDepth) : -1,
                    zox_valid(mesh) ? zox_getn(mesh) : "none", zox_valid(mesh) && zox_has(mesh, MeshVertices) ? zox_get(mesh, MeshVertices)->length : -1);
                const entity c = zox_tst_fplace_chunk;
                zox_log("[tst] chunk [%s] depth [%i] blocks spawned [%i] render disabled [%i] distance [%i] edited [%i] at y [%i]", zox_getn(c),
                    zox_getv(c, NodeDepth), zox_has(c, BlocksSpawned), zox_getv(c, RenderDisabled), zox_getv(c, RenderDistance), zox_has(c, Edited),
                    zox_tst_fplace_at.y);
                return 1;
            }
            return 0;
        }
    }
    zox_tst_fplace_stage = 2;
    return 1;
}

void zox_tst_furniture_place_check(ecs* world) {
    zox_tst_check(zox_tst_fplace_stage == 2, "the flower, bed and door never all had a block vox with a mesh");
    if (zox_tst_fplace_stage != 2) {
        return;
    }
    const entity character = zox_tst_player_character(world);
    const float block = zox_getv(zox_get_link(world, character, TerrainLink), BlockScale);
    const entity chunk = zox_tst_fplace_chunk;
    const byte chunk_depth = zox_has(chunk, RenderDepth) ? zox_getv(chunk, RenderDepth) : 0;
    float3 positions[3];
    for (byte i = 0; i < 3; i++) {
        const byte3 at = { zox_tst_fplace_at.x + i * 4, zox_tst_fplace_at.y, zox_tst_fplace_at.z };
        const entity vode = zox_tst_fplace_vode(world, chunk, at);
        const entity mesh = zox_get_link(world, vode, InstanceLink);
        const MeshVertices* vertices = zox_get(mesh, MeshVertices);
        float3 min = vertices->value[0], max = vertices->value[0];
        for (int j = 1; j < vertices->length; j++) {
            const float3 v = vertices->value[j];
            min = (float3) { v.x < min.x ? v.x : min.x, v.y < min.y ? v.y : min.y, v.z < min.z ? v.z : min.z };
            max = (float3) { v.x > max.x ? v.x : max.x, v.y > max.y ? v.y : max.y, v.z > max.z ? v.z : max.z };
        }
        positions[i] = zox_getv(vode, Position3D);
        const float3 p = positions[i];
        const byte depth = zox_getv(vode, RenderDepth);
        zox_log("[tst] [metric] %s at [%.3f %.3f %.3f] render depth [%i] (chunk [%i]) mesh [%.3f %.3f %.3f] to [%.3f %.3f %.3f] in blocks", i == 0 ? "flower" :
            i == 1 ? "bed" : "door", p.x, p.y, p.z, depth, chunk_depth, min.x / block, min.y / block, min.z / block, max.x / block, max.y / block, max.z / block);
        if (!i) {
            continue;
        }
        // half a block further along every axis than the one block model, 4 voxels along x a model
        const float3 expected = { positions[0].x + (i * 4 + 0.5f) * block, positions[0].y + 0.5f * block, positions[0].z + 0.5f * block };
        zox_tst_check(fabsf(p.x - expected.x) < 0.01f && fabsf(p.y - expected.y) < 0.01f && fabsf(p.z - expected.z) < 0.01f,
            "the %s is at [%.3f %.3f %.3f], not [%.3f %.3f %.3f]: its cube's corner isn't at its base voxel's", i == 1 ? "bed" : "door",
            p.x, p.y, p.z, expected.x, expected.y, expected.z);
        // its mesh from the cube's low corner (a block below its middle), two blocks along x (bed) or y (door)
        const float long_axis = i == 1 ? max.x - min.x : max.y - min.y;
        const float short_axis = i == 1 ? max.y - min.y : max.x - min.x;
        zox_tst_check(fabsf(min.x + block) < 0.05f * block && fabsf(min.y + block) < 0.05f * block, "the %s's mesh starts at [%.2f %.2f] blocks, not its cube's corner [-1 -1]",
            i == 1 ? "bed" : "door", min.x / block, min.y / block);
        zox_tst_check(fabsf(long_axis - 2 * block) < 0.05f * block, "the %s is [%.2f] blocks long, not 2", i == 1 ? "bed" : "door", long_axis / block);
        zox_tst_check(short_axis <= 1.01f * block, "the %s is [%.2f] blocks across, more than 1", i == 1 ? "bed" : "door", short_axis / block);
        zox_tst_check(depth == zox_getv(zox_tst_fplace_vode(world, chunk, zox_tst_fplace_at), RenderDepth) + 1,
            "the %s's render depth is [%i], not a depth past a one block model's", i == 1 ? "bed" : "door", depth);
    }
}

void zox_tsts_furniture(ecs* world) {
    zox_tst_add("furniture_place", zox_tst_furniture_place_ready, zox_tst_furniture_place_check, 120);
}
