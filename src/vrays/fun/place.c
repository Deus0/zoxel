// this is for spawninng voxel
//      should take in event for future voxels
// note: it only supports placing and removing, not swapping

void raycast_action(
    ecs *world,
    const RaycastVoxelData data,
    byte voxel_index,
    byte hit_type
) {
    byte3 positionl = hit_type == 2 ? data.positionl : data.positionl_last;

    entity chunk = hit_type == 2 ? data.chunk : data.chunk_last;

    if (!zox_valid(chunk)) {
        zox_log_error("hit_type [%i] - chunk invalid inside [raycast_action]", hit_type);
        return;
    }

    // add to queue
    zox_muter(chunk, VoxelNodeQueue, queue);
    a_VoxelNodeQueue(queue,
        (VoxelNodeUpdate) {
            .value = voxel_index,
            .pos = positionl,
        });
}
