// this is for spawninng voxel
//      should take in event for future voxels
// note: it only supports placing and removing, not swapping


/*void place_block(
    ecs *world,
    const entity chunk,
    VoxelNode *node,
    const byte3 positionl,
    const byte voxel
    //const float3 positionf
) {
    if (!node || !zox_valid(chunk) || !zox_has(chunk, VoxelNode) || !zox_has(chunk, NodeDepth)) {
        return;
    }
    zox_geter_value(chunk, NodeDepth, byte, node_depth);
    zox_muter(chunk, VoxelNode, base_node);
    const SetVoxelTargetData datam = {
        .depth = node_depth,
        .voxel = voxel,
        .effect_nodes = 1,
    };
    SetVoxelData data2 = {
        .node = base_node,
        .position = positionl,
    };
    node = set_voxel(datam, data2);

    // - Refresh Meshes
    zox_set(chunk, VoxelNodeEdited, { 1 }); // now its edited!
    zox_set(chunk, VoxelNodeDirty, { zox_dirty_trigger });
}*/


void raycast_action(
    ecs *world,
    const RaycastVoxelData *data,
    const byte voxel,
    byte hit_type
) {
    byte3 positionl = hit_type == 2 ? data->positionl : data->positionl_last;
    entity chunk = hit_type == 2 ? data->chunk : data->chunk_last;
    // VoxelNode* node = hit_type == 2 ? data->node : data->node_last;
    // place_block(world, chunk, node, positionl, voxel);

    // add to queue
    zox_muter(chunk, VoxelNodeQueue, queue);
    a_VoxelNodeQueue(queue,
        (VoxelNodeUpdate) {
            .value = voxel,
            .pos = positionl
        });
}
