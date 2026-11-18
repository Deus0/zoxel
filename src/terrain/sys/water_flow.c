// Flowing water: levels, Minecraft style (doc/tech/water.md Flowing)
//  - water (BlockWater) is a source, level 8: it never moves. Flowing water (BlockWaterFlowing) is a block a level,
//      7 down to 1 (BlockWaterLevel)
//  - a block's water is what flows into it: from water over it, 7 (it falls); else from the water beside it that
//      lies on something (not air, not flowing), a level less than it (a source gives 7)
//  - water_flow_edits_system: a chunk just edited (VoxelNodePostDirty) adds its edited blocks and those round them
//      to its terrain's WaterFlows; flowing water's own edits (flowed) don't, it adds its own
//  - water_flow_system: a few flows a frame, each when its time comes: a block of air or flowing water takes the
//      level flowing into it (filling, rising, or lowering: water with nothing feeding it drains a level a step), and
//      when it changed, the blocks round it are checked a step later
#define zox_water_flow_time 0.25
#define zox_water_flows_max 2048
#define zox_water_flows_a_frame 64

// adds a block to check at a time; one already waiting waits till then (it'd check before a change round it lands)
static inline void water_flow_add(WaterFlows* flows, int3 block, double at) {
    for (int i = 0; i < flows->length; i++) {
        if (int3_equals(flows->value[i].block, block)) {
            if (flows->value[i].at < at) {
                flows->value[i].at = at;
            }
            return;
        }
    }
    if (flows->length >= zox_water_flows_max) {
        return;
    }
    const int at_index = flows->length;
    resize_WaterFlows(flows, at_index + 1);
    flows->value[at_index] = (WaterFlow) { block, at };
}

static const int3 water_flow_around[6] = { { 1, 0, 0 }, { -1, 0, 0 }, { 0, 0, 1 }, { 0, 0, -1 }, { 0, -1, 0 }, { 0, 1, 0 } };

// a block and the six round it, to check at a time
static inline void water_flow_add_around(WaterFlows* flows, int3 block, double at) {
    water_flow_add(flows, block, at);
    for (byte k = 0; k < 6; k++) {
        water_flow_add(flows, int3_add(block, water_flow_around[k]), at);
    }
}

// the level water flows into a block from round it, 0 none
static inline byte water_flow_into(ecs* world, entity terrain, const WaterLevels* levels, int3 block) {
    if (voxel_is_water(levels, terrain_voxel_at(world, terrain, int3_add(block, int3_up)))) {
        return zox_water_level_source - 1;
    }
    byte into = 0;
    for (byte k = 0; k < 4; k++) {
        const int3 side = int3_add(block, water_flow_around[k]);
        const int voxel = terrain_voxel_at(world, terrain, side);
        if (!voxel_is_water(levels, voxel)) {
            continue;
        }
        // water spreads where it lies on something: falling water (over air or flowing water) doesn't
        const int under = terrain_voxel_at(world, terrain, int3_add(side, int3_down));
        if (under <= 0 || (voxel_is_water(levels, under) && levels->level[under] < zox_water_level_source)) {
            continue;
        }
        const byte level = levels->level[voxel] - 1;
        if (level > into) {
            into = level;
        }
    }
    return into > zox_water_level_source - 1 ? zox_water_level_source - 1 : into;
}

// queues a voxel at a world block position on its chunk, 0 if its chunk isn't loaded
static inline byte water_flow_set(ecs* world, entity terrain, int3 block, byte value) {
    const byte depth = zox_getv(terrain, NodeDepth);
    const int length = octree_size(depth);
    const int3 chunk_position = block_position_to_chunk_position(block, depth);
    const entity chunk = int3_hashmap_get(zox_get(terrain, ChunkLinks)->value, chunk_position);
    if (!zox_valid(chunk) || !zox_has(chunk, VoxelNodeQueue)) {
        return 0;
    }
    const byte3 local = { block.x - chunk_position.x * length, block.y - chunk_position.y * length, block.z - chunk_position.z * length };
    VoxelNodeQueue* queue = zox_getm(chunk, VoxelNodeQueue);
    a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = value, .position = local, .flowed = 1 });
    return 1;
}

void water_flow_edits_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ChunkPosition);
    zox_sys_in(NodeDepth);
    zox_sys_in(VoxelNodeQueue);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ChunkPosition, chunk_position);
        zox_sys_i(NodeDepth, depth);
        zox_sys_i(VoxelNodeQueue, queue);
        const entity terrain = zox_get_parent(world, e);
        if (!zox_valid(terrain) || !zox_has(terrain, WaterFlows) || depth->value != zox_getv(terrain, NodeDepth)) {
            continue;
        }
        WaterFlows* flows = zox_getm(terrain, WaterFlows);
        const int length = octree_size(depth->value);
        for (int j = 0; j < queue->count; j++) {
            const VoxelNodeUpdate update = queue->ptr[j];
            if (update.flowed || update.state == zox_voxel_queue_edit) {
                continue;
            }
            const int3 block = { chunk_position->value.x * length + update.position.x, chunk_position->value.y * length + update.position.y,
                chunk_position->value.z * length + update.position.z };
            water_flow_add_around(flows, block, zox_current_time + zox_water_flow_time);
        }
    }
    zox_sys_on_end();
} zoxd_system(water_flow_edits_system);

void water_flow_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(WaterFlows);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(WaterFlows, flows);
        if (!flows->length) {
            continue;
        }
        WaterLevels levels;
        if (!water_levels_get(world, zox_get_parent(world, e), &levels)) {
            resize_WaterFlows(flows, 0);
            continue;
        }
        const double next = zox_current_time + zox_water_flow_time;
        int done = 0;
        for (int j = 0; j < flows->length && done < zox_water_flows_a_frame;) {
            const WaterFlow flow = flows->value[j];
            if (flow.at > zox_current_time) {
                j++;
                continue;
            }
            // taken out (the last moved into its place), then done
            flows->value[j] = flows->value[flows->length - 1];
            resize_WaterFlows(flows, flows->length - 1);
            done++;
            const int voxel = terrain_voxel_at(world, e, flow.block);
            // air or flowing water: sources and solids stay
            const byte level = voxel_is_water(&levels, voxel) ? levels.level[voxel] : 0;
            if (voxel < 0 || (voxel && !level) || level == zox_water_level_source) {
                continue;
            }
            const byte into = water_flow_into(world, e, &levels, flow.block);
            if (into == level) {
                continue;
            }
            if (water_flow_set(world, e, flow.block, levels.voxel[into])) {
                water_flow_add_around(flows, flow.block, next);
            }
        }
    }
    zox_sys_on_end();
} zoxd_system(water_flow_system);
