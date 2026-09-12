static void debug_octree_node_compare(
    ecs* world,
    const LightNode* node,
    const VoxelNode* compare_node,
    byte depth,
    byte target_depth,
    float3 position,
    float scale,
    byte ignore_value
) {
    if (!node) {
        zox_log_error("invalid node in debug_octree_node");
        return;
    }
    const LightNode** kids_ptr = (const LightNode**) node;
    const VoxelNode** compare_kids =
        debugger_is_ignore_voxel && compare_node
            ? (const VoxelNode**) compare_node
            : NULL;

    if (depth == target_depth || !*kids_ptr) {
        // Ignore if solid
        if (debugger_is_ignore_voxel && compare_node && compare_node->value) {
            return;
        }

        byte v = node->value;
        color_rgb col = { v, v, v };
        float3 size = float3_single(scale * debug_octree_padding);
        float3 rsize = float3_single(scale);
        float3 center = float3_add(position, float3_half(rsize));
        debug_cubec(world, center, size, col, debug_octree_thickness);
        return;
    }

    const VoxelNode* child_compare = compare_node;
    float child_scale = scale * 0.5f;

    for (byte i = 0; i < 8; i++) {
        const LightNode* child =
            (const LightNode*)((const char*)(*kids_ptr) + i * sizeof(LightNode));

        if (!child) {
            zox_log_error("invalid child on octree");
            continue;
        }

        byte3 offset = octree_positions[i];

        float3 child_pos = float3_add(
            position,
            (float3) {
                offset.x * child_scale,
                offset.y * child_scale,
                offset.z * child_scale
            }
        );

        if (compare_kids && *compare_kids) {
            child_compare =
                (const VoxelNode*)((const char*)(*compare_kids) + i * sizeof(VoxelNode));
        }

        debug_octree_node_compare(
            world,
            child,
            child_compare,
            depth + 1,
            target_depth,
            child_pos,
            child_scale,
            ignore_value
        );
    }
}

static inline void debug_octree_compare(
    ecs* world,
    const LightNode* root,
    const VoxelNode* compare_root,
    byte target_depth,
    float3 root_position,
    float root_scale,
    byte ignore_value
) {
    debug_octree_node_compare(
        world,
        root,
        compare_root,
        0,
        target_depth,
        root_position,
        root_scale,
        ignore_value
    );
}

// NOTE: This compares voxel nodes to make sure it only renders cubes for air blocks
zox_sys2(LightNodeDebugSystem) {
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
    zox_sys_in(LightNode);
    zox_sys_in(RenderDepth);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Position3D, position);
        zox_sys_i(BlockScale, scale);
        zox_sys_i(RenderDistance, distance);
        zox_sys_i(ChunkPosition, positionc);
        zox_sys_i(VoxelNode, vnode);
        zox_sys_i(LightNode, lights);
        zox_sys_i(RenderDepth, depth);
        // NOTE: Only debugs inside of chunk
        if (distance->value > 0 || !(positionc->value.y >= 0 && positionc->value.y <= 4)) {
            continue;
        }
        // TODO: Limit to distance from camera
        float chunk_scale = scale->value * powers_of_two[depth->value];
        // zox_log("debugin chunk at scale [%f]", chunk_scale);
        // debug_octree_compare_LightNode(world, lights, vnode, debug_depth, position->value, chunk_scale, sunlight);
        debug_octree_compare(world, lights, vnode, debug_depth, position->value, chunk_scale, sunlight);
        /*if (!lights->value && !lights->ptr) {
            zox_loge("Lights Dark [%s]", zox_sys_e_name);
        }*/
    }
} zox_sys_end(LightNodeDebugSystem);
