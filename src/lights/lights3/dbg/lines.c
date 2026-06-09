// NOTE: This compares voxel nodes to make sure it only renders cubes for air blocks
zox_sys2(LightNodeDebugSystem) {
    if (!zox_debug_lights) {
        return;
    }
    byte debug_depth = zox_debug_lights - 1;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Position3D);
    zox_sys_in(BlockScale);
    zox_sys_in(RenderDistance);
    zox_sys_in(ChunkPosition);
    zox_sys_in(VoxelNode);
    zox_sys_in(LightNode);
    zox_sys_in(RenderDepth);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Position3D, position);
        zox_sys_i(BlockScale, scale);
        zox_sys_i(RenderDistance, distance);
        zox_sys_i(ChunkPosition, positionc);
        zox_sys_i(VoxelNode, vnode);
        zox_sys_i(LightNode, lnode);
        zox_sys_i(RenderDepth, depth);
        // NOTE: Only debugs inside of chunk
        if (distance->value > 0 || !(positionc->value.y >= 0 && positionc->value.y <= 4)) {
            continue;
        }
        // TODO: Limit to distance from camera
        float chunk_scale = scale->value * powers_of_two[depth->value];
        // zox_log("debugin chunk at scale [%f]", chunk_scale);
        debug_octree_compare_LightNode(world, lnode, vnode, debug_depth, position->value, chunk_scale, sunlight);
    }
} zox_sys_end(LightNodeDebugSystem);
