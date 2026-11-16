// Flowing water: an edit next to water (a block dug out by a lake) or water placed spreads (doc/tech/water.md)
//  - water_flow_edits_system: a chunk just edited (VoxelNodePostDirty) adds its edited blocks to its terrain's
//      WaterFlows; flowing water's own edits (flowed) don't, so it can't spread on forever
//  - water_flow_system: a few flows a frame, each when its time comes: air with water over it or beside it fills;
//      water falls into air under it, else spreads sideways to air beside it, at most zox_water_flow_steps from
//      where the flow began; a step every zox_water_flow_time
#define zox_water_flow_steps 4
#define zox_water_flow_time 0.25
#define zox_water_flows_max 2048
#define zox_water_flows_a_frame 64

// adds a block to check at a time, unless it's already waiting
static inline void water_flow_add(WaterFlows* flows, int3 block, byte steps, double at) {
    for (int i = 0; i < flows->length; i++) {
        if (int3_equals(flows->value[i].block, block)) {
            if (steps < flows->value[i].steps) {
                flows->value[i].steps = steps;
            }
            return;
        }
    }
    if (flows->length >= zox_water_flows_max) {
        return;
    }
    const int at_index = flows->length;
    resize_WaterFlows(flows, at_index + 1);
    flows->value[at_index] = (WaterFlow) { block, steps, at };
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

static const int3 water_flow_sides[4] = { { 1, 0, 0 }, { -1, 0, 0 }, { 0, 0, 1 }, { 0, 0, -1 } };

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
            water_flow_add(flows, block, 0, zox_current_time + zox_water_flow_time);
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
        const entity water = zox_get_child_by_id(world, zox_get_parent(world, e), zox_id(BlockWater));
        if (!zox_valid(water)) {
            resize_WaterFlows(flows, 0);
            continue;
        }
        const int water_id = zox_getv(water, BlockIndex);
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
            if (voxel == 0) {
                // air fills from water over it or beside it, then flows on from there
                byte wet = terrain_voxel_at(world, e, int3_add(flow.block, int3_up)) == water_id;
                for (byte k = 0; k < 4 && !wet; k++) {
                    wet = terrain_voxel_at(world, e, int3_add(flow.block, water_flow_sides[k])) == water_id;
                }
                if (wet && water_flow_set(world, e, flow.block, water_id)) {
                    water_flow_add(flows, flow.block, flow.steps, next);
                }
                continue;
            }
            if (voxel != water_id) {
                continue;
            }
            // water falls into air under it, else spreads to air beside it
            const int3 below = int3_add(flow.block, int3_down);
            const int under = terrain_voxel_at(world, e, below);
            if (under == 0) {
                if (water_flow_set(world, e, below, water_id)) {
                    water_flow_add(flows, below, flow.steps, next);
                }
                continue;
            }
            if (flow.steps >= zox_water_flow_steps) {
                continue;
            }
            for (byte k = 0; k < 4; k++) {
                const int3 side = int3_add(flow.block, water_flow_sides[k]);
                if (terrain_voxel_at(world, e, side) == 0 && water_flow_set(world, e, side, water_id)) {
                    water_flow_add(flows, side, flow.steps + 1, next);
                }
            }
        }
    }
    zox_sys_on_end();
} zoxd_system(water_flow_system);
