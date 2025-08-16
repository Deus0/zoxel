void LightNodeDebugSystem(iter *it) {

    if (!zox_debug_lights) return;

    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Position3D);
    zox_sys_in(BlockScale);
    zox_sys_in(RenderDistance);
    zox_sys_in(ChunkPosition);
    zox_sys_in(VoxelNode);
    zox_sys_in(LightNode);
    zox_sys_in(LightNodeDepth);

    for (int i = 0; i < it->count; i++) {

        zox_sys_i(Position3D, position);
        zox_sys_i(BlockScale, scale);
        zox_sys_i(RenderDistance, distance);
        zox_sys_i(ChunkPosition, positionc);
        zox_sys_i(VoxelNode, vnode);
        zox_sys_i(LightNode, lnode);
        zox_sys_i(LightNodeDepth, depth);

        if (distance->value > 1 || !(positionc->value.y >= 0 && positionc->value.y <= 2)) {
            continue;
        }

        // TODO: Limit to distance from camera

        byte chunk_length = powers_of_two[depth->value];
        const float chunk_scale = scale->value * chunk_length;

        debug_octree_compare_LightNode(
            world,
            lnode,
            vnode,
            depth->value,
            position->value,
            chunk_scale,
            sunlight
        );

    }
} zoxd_system2(LightNodeDebugSystem);
