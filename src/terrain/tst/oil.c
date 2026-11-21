// tst: oil_flow, oil is a liquid as water is, its own (terrain/fun/water.c liquids, zox_block_material_oil)
//  - the realm has oil: its own material, walked through, light passing; its flowing blocks link it (LiquidLink)
//  - no player: in an all air chunk (air_chunk.c), a floor of bricks 13 x 13 (local y 3), an oil source 2 over it at
//    x 3 and a water source at x 11 (z 7)
//  - each falls and spreads: the floor's blocks near each are their own liquid a level less a block (oil by the oil,
//    water by the water), neither turns into the other
//  - the oil draws its own see through mesh, its texture the oil's (not water's)
//  - run: bash bsh/tst.sh oil_flow
byte zox_tst_oil_stage = 0;
double zox_tst_oil_time = 0;
entity zox_tst_oil_chunk = 0;
int zox_tst_oil_wrong = -1, zox_tst_oil_mixed = -1;
byte zox_tst_oil_mesh = 0, zox_tst_oil_texture = 0;

byte zox_tst_oil_flow_ready(ecs* world) {
    zox_tst_world_ready(world);
    const entity terrain = zox_tst_world_terrain;
    if (!terrain) {
        return 0;
    }
    zox_tst_progress(zox_tst_oil_stage * 100 + (int) (zox_current_time - zox_tst_oil_time));
    const entity realm = zox_tst_world_realm;
    const entity oil = zox_get_child_by_id(world, realm, zox_id(BlockOil));
    const entity water = zox_get_child_by_id(world, realm, zox_id(BlockWater));
    if (zox_tst_oil_stage == 0) {
        zox_tst_oil_chunk = zox_tst_streamed_air_chunk(world);
        if (!zox_tst_oil_chunk) {
            return 0;
        }
        zox_tst_check(zox_valid(oil) && zox_valid(water), "the realm has no oil [%i] or water [%i]", zox_valid(oil), zox_valid(water));
        if (!zox_valid(oil) || !zox_valid(water)) {
            return 1;
        }
        const entity bricks = zox_get_child_by_id(world, realm, zox_id(BlockBricks));
        VoxelNodeQueue* queue = zox_getm(zox_tst_oil_chunk, VoxelNodeQueue);
        for (int x = 1; x <= 13; x++) for (int z = 1; z <= 13; z++) {
            a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = zox_getv(bricks, BlockIndex), .position = (byte3) { x, 3, z } });
        }
        zox_tst_oil_stage = 1;
        zox_tst_oil_time = zox_current_time;
        return 0;
    }
    if (zox_tst_oil_stage == 1) {
        zox_tst_waiting = "the floor, then the oil and water dropped on it";
        if (zox_current_time - zox_tst_oil_time < 1) {
            return 0;
        }
        VoxelNodeQueue* queue = zox_getm(zox_tst_oil_chunk, VoxelNodeQueue);
        a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = zox_getv(oil, BlockIndex), .position = (byte3) { 3, 6, 7 } });
        a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = zox_getv(water, BlockIndex), .position = (byte3) { 11, 6, 7 } });
        zox_tst_oil_stage = 2;
        zox_tst_oil_time = zox_current_time;
        return 0;
    }
    zox_tst_waiting = "the oil and water to spread";
    if (zox_current_time - zox_tst_oil_time < 5) {
        return 0;
    }
    WaterLevels levels;
    water_levels_get(world, realm, &levels);
    const byte oil_liquid = levels.liquid[zox_getv(oil, BlockIndex)];
    const byte water_liquid = levels.liquid[zox_getv(water, BlockIndex)];
    const VoxelNode* voxels = zox_get(zox_tst_oil_chunk, VoxelNode);
    const byte depth = zox_getv(zox_tst_oil_chunk, NodeDepth);
    zox_tst_oil_wrong = zox_tst_oil_mixed = 0;
    // the floor's blocks within 3 of each source (Manhattan), each its own liquid at its level
    for (int x = 1; x <= 13; x++) for (int z = 1; z <= 13; z++) {
        const int to_oil = abs(x - 3) + abs(z - 7), to_water = abs(x - 11) + abs(z - 7);
        const byte voxel = getv_VoxelNode(voxels, depth, (byte3) { x, 4, z });
        if (to_oil <= 3 || to_water <= 3) {
            const byte liquid = to_oil <= 3 ? oil_liquid : water_liquid;
            const byte level = zox_water_level_source - 1 - (to_oil <= 3 ? to_oil : to_water);
            if (!voxel_is_water(&levels, voxel) || levels.level[voxel] != level) {
                zox_tst_oil_wrong++;
            } else if (levels.liquid[voxel] != liquid) {
                zox_tst_oil_mixed++;
            }
        }
    }
    // its own see through mesh, the oil's texture
    const entity oil_texture = zox_get_child_by_id(world, oil, zox_id(Texture));
    entity meshes[zox_chunk_meshes_max];
    const uint count = get_chunk_meshes(world, zox_tst_oil_chunk, meshes);
    for (uint i = 0; i < count; i++) {
        const entity material = zox_get_parent(world, meshes[i]);
        if (zox_valid(material) && zox_has(material, BlockMaterial) && zox_getv(material, BlockMaterial) == zox_block_material_oil &&
            zox_has(meshes[i], TransparentMesh) && !zox_has(meshes[i], Disabled))
        {
            zox_tst_oil_mesh = 1;
            zox_tst_oil_texture |= zox_valid(oil_texture) && zox_get_link(world, meshes[i], MeshTextureLink) == oil_texture;
        }
    }
    zox_tst_oil_stage = 3;
    return 1;
}

void zox_tst_oil_flow_check(ecs* world) {
    const entity oil = zox_get_child_by_id(world, zox_tst_world_realm, zox_id(BlockOil));
    if (!zox_valid(oil)) {
        return;
    }
    zox_tst_check(zox_getv(oil, BlockMaterial) == zox_block_material_oil, "oil isn't its own material");
    zox_tst_check(zox_getv(oil, BlockCollider) == zox_block_air, "oil isn't walked through");
    zox_tst_check(zox_getv(oil, BlockLightPass), "oil stops light");
    zox_tst_check(zox_tst_oil_stage == 3, "no all air chunk at full detail was found");
    zox_log("[tst] [metric] oil_flow: [%i] floor blocks near a source not their level, [%i] the other liquid, its mesh [%i] its texture [%i]",
        zox_tst_oil_wrong, zox_tst_oil_mixed, zox_tst_oil_mesh, zox_tst_oil_texture);
    zox_tst_check(!zox_tst_oil_wrong, "[%i] floor blocks near a source aren't its liquid a level less a block", zox_tst_oil_wrong);
    zox_tst_check(!zox_tst_oil_mixed, "[%i] floor blocks near a source are the other liquid", zox_tst_oil_mixed);
    zox_tst_check(zox_tst_oil_mesh, "the oil drew no see through mesh of its own");
    zox_tst_check(zox_tst_oil_texture, "the oil's mesh doesn't draw the oil's texture");
}
