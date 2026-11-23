byte debugger_is_ignore_voxel = 0;  // only air voxels' lights

// NOTE: This compares voxel nodes to make sure it only renders cubes for air blocks
void light_octree_debug_system(iter* it) {
    zox_sys_on_begin();
    /*if (!zox_debug_lights) {
        return;
    }*/
    byte debug_depth = zox_debug_lights - 1;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Position3D);
    zox_sys_in(BlockScale);
    zox_sys_in(RenderDistance);
    zox_sys_in(ChunkPosition);
    zox_sys_in(VoxelNode);
    zox_sys_in(LightOctree);
    zox_sys_in(RenderDepth);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Position3D, position);
        zox_sys_i(BlockScale, scale);
        zox_sys_i(RenderDistance, distance);
        zox_sys_i(ChunkPosition, positionc);
        zox_sys_i(VoxelNode, vnode);
        zox_sys_i(LightOctree, lights);
        zox_sys_i(RenderDepth, depth);
        // NOTE: the camera's chunk and a chunk round it
        if (distance->value > 1 || !(positionc->value.y >= 0 && positionc->value.y <= 4)) {
            continue;
        }
        // TODO: Limit to distance from camera
        float chunk_scale = scale->value * powers_of_two[depth->value];
        // zox_log("debugin chunk at scale [%f]", chunk_scale);
        const debug_octree_style style = { sizeof(LightOctree), octree_type_offset_LightOctree, sizeof(VoxelNode),
            octree_type_offset_VoxelNode, debug_depth, -1, debugger_is_ignore_voxel, octree_debug_grey };
        debug_octree(world, &style, lights, vnode, position->value, chunk_scale);
        /*if (!lights->value && !lights->ptr) {
            zox_loge("Lights Dark [%s]", zox_sys_e_name);
        }*/
    }
    zox_sys_on_end();
} zoxd_system(light_octree_debug_system);
