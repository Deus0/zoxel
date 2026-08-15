// NOTE: When light resolution increases, we need to reflood lights from nearby chunks
/*byte try_flood_light(const byte* solidity, const VoxelNode* voxels, LightNode* lights, byte depth, byte3 position, const VoxelNode* neighbor_voxels, const LightNode* neighbor_lights, byte3 neighbor_position, const VoxelNode** nvoxels, const LightNode** nlights, LightQueue** nqueues) {
    // first check this chunks voxel
    byte voxel = getv_VoxelNode(voxels, depth, position);
    if (voxel && solidity[voxel - 1]) {
        return 0;
    }
    byte neighbor_voxel = getv_VoxelNode(neighbor_voxels, depth, neighbor_position);
    if (neighbor_voxel && solidity[neighbor_voxel - 1]) {
        return 0;
    }
    byte neighbor_light = getv_LightNode(neighbor_lights, depth, neighbor_position);
    neighbor_light -= light_air_decay;
    return flood_light(voxels, lights, nvoxels, nlights, nqueues, depth, position, neighbor_light, light_propogation_distance, darklight, light_air_decay, solidity);
}

zox_sys2(RefreshLightsSystem) {
    // byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(BlockManagerLink);
    zox_sys_in(VoxelNode);
    zox_sys_in(ChunkNeighbors);
    zox_sys_out(RefreshLights);
    zox_sys_out(LightNode);
    zox_sys_out(LightNodeDirty);
    entity realm = 0;
    byte solidity[255];
    for (int j = 0; j < 255; j++) {
        solidity[j] = 1;
    }
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(BlockManagerLink, manager);
        zox_sys_i(VoxelNode, voxels);
        zox_sys_i(ChunkNeighbors, neighbors);
        zox_sys_o(RefreshLights, refresh);
        zox_sys_o(LightNode, lights);
        zox_sys_o(LightNodeDirty, light_node_dirty);
        if (refresh->value != zox_refresh_lights) {
            continue;
        }
        if (zox_has(e, GenerateChunk)) {
            continue;
        }
        // NOTE: Check Blocks Caches
        if (realm != manager->value) {
            realm = manager->value;
            zox_geter(realm, BlockLinks, blocks);
            for (int j = 0; j < blocks->length; j++) {
                entity block = blocks->value[j];
                solidity[j] = zox_valid(block) && zox_has(block, BlockLightPass) ? !zox_getv(block, BlockLightPass) : 1;
            }
        }
        const VoxelNode* nvoxels[6];
        fetch_neightbor_voxel_nodes(world, neighbors, nvoxels);
        const LightNode* nlights[6];
        fetch_neightbor_light_nodes(world, neighbors, nlights);
        LightQueue* nqueues[6];
        fetch_neightbor_propogation_queues(world, neighbors, nqueues);
        byte dirty = 0;
        byte depth = zox_getv(e, NodeDepth);
        short length = octree_size(depth);
        // First flood in from back chunk
        entity neighbor = neighbors->value[direction_back];
        if (zox_valid(neighbor) && zox_getv(neighbor, NodeDepth) == depth) {
            const VoxelNode* neighbor_voxels = nvoxels[direction_back];
            const LightNode* neighbor_lights = nlights[direction_back];
            byte3 position;
            byte3 neighbor_position;
            neighbor_position.z = length - 1;
            position.z = 0;
            for (position.x = 0; position.x < length; position.x++) {
                for (position.y = 0; position.y < length; position.y++) {
                    neighbor_position.x = position.x;
                    neighbor_position.y = position.y;
                    if (try_flood_light(solidity, voxels,lights, depth, position, neighbor_voxels, neighbor_lights, neighbor_position, nvoxels,  nlights, nqueues)) {
                        dirty = 1;
                    }
                }
            }
        }
        neighbor = neighbors->value[direction_front];
        if (zox_valid(neighbor) && zox_getv(neighbor, NodeDepth) == depth) {
            const VoxelNode* neighbor_voxels = nvoxels[direction_front];
            const LightNode* neighbor_lights = nlights[direction_front];
            byte3 position;
            byte3 neighbor_position;
            neighbor_position.z = 0;
            position.z = length - 1;
            for (position.x = 0; position.x < length; position.x++) {
                for (position.y = 0; position.y < length; position.y++) {
                    neighbor_position.x = position.x;
                    neighbor_position.y = position.y;
                    if (try_flood_light(solidity, voxels,lights, depth, position, neighbor_voxels, neighbor_lights, neighbor_position, nvoxels,  nlights, nqueues)) {
                        dirty = 1;
                    }
                }
            }
        }
        neighbor = neighbors->value[direction_left];
        if (zox_valid(neighbor) && zox_getv(neighbor, NodeDepth) == depth) {
            const VoxelNode* neighbor_voxels = nvoxels[direction_left];
            const LightNode* neighbor_lights = nlights[direction_left];
            byte3 position;
            byte3 neighbor_position;
            neighbor_position.x = length - 1;
            position.x = 0;
            for (position.y = 0; position.y < length; position.y++) {
                for (position.z = 0; position.z < length; position.z++) {
                    neighbor_position.y = position.y;
                    neighbor_position.z = position.z;
                    if (try_flood_light(solidity, voxels,lights, depth, position, neighbor_voxels, neighbor_lights, neighbor_position, nvoxels,  nlights, nqueues)) {
                        dirty = 1;
                    }
                }
            }
        }
        neighbor = neighbors->value[direction_right];
        if (zox_valid(neighbor) && zox_getv(neighbor, NodeDepth) == depth) {
            const VoxelNode* neighbor_voxels = nvoxels[direction_right];
            const LightNode* neighbor_lights = nlights[direction_right];
            byte3 position;
            byte3 neighbor_position;
            neighbor_position.x = 0;
            position.x = length - 1;
            for (position.y = 0; position.y < length; position.y++) {
                for (position.z = 0; position.z < length; position.z++) {
                    neighbor_position.y = position.y;
                    neighbor_position.z = position.z;
                    if (try_flood_light(solidity, voxels,lights, depth, position, neighbor_voxels, neighbor_lights, neighbor_position, nvoxels,  nlights, nqueues)) {
                        dirty = 1;
                    }
                }
            }
        }
        if (dirty) {
            light_node_dirty->value = zox_dirty_trigger;
        }
        refresh->value = 0;
    }
} zox_sys_end(RefreshLightsSystem);
*/
