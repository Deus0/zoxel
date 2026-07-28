// hmmm issue seems to be about faces
// maybe we redo our mesh builder system

static inline void zox_apply_smooth_lights(const LightNode** lights, const VoxelNode* voxels, const SidesOctree* sides, const MeshColorRGBs* colors, byte3 position, uint* ccount, byte target_depth, byte depth) {
    // Dig Deeper
    if (depth < target_depth && sides->ptr) {
        const SidesOctree* sides_kids = (const SidesOctree*) sides->ptr;
        byte has_vkids = !is_closed_VoxelNode(voxels);
        const VoxelNode* vkids = has_vkids ? get_children_VoxelNode(voxels) : NULL;
        byte3_multiply_byte(&position, 2);
        depth++;
        for (byte i = 0; i < 8; i++) {
            const VoxelNode* cvoxels = has_vkids ? &vkids[i] : voxels;
            byte3 child_position = byte3_add(position, octree_positions_b[i]);
            zox_apply_smooth_lights(lights, cvoxels, &sides_kids[i], colors, child_position, ccount, target_depth, depth);
//#ifdef zox_safety_checks
            if (*ccount + voxel_face_vertices_length > colors->length) {
                break;
            }
// #endif
        }
        return;
    }
    // NOTE: If no sides were rendered
    if (!sides->value) {
        return;
    }
    if (!voxels->value) {
        return;
    }
    // TODO: Handle Corners Properly:
    //      - If two adjacent voxels solid, dont use corner light as it will be blocked
    // for each face that is visible according to node->sides
    for (byte direction = 0; direction < 6; direction++) {
        // skip hidden face
        if (!(sides->value & (1 << (direction + 1)))) {
            continue;
        }
//#ifdef zox_safety_checks
        if (*ccount + voxel_face_vertices_length > colors->length) {
            zox_loge("  - Colors Past Limits [%i] - Face [%i] at Pos [%ix%ix%i] Depth [%i] of [%i]", *ccount, direction, position.x, position.y, position.z, depth, target_depth);
            return;
        }
//#endif
        byte adjacent_light = getv_nearby_LightNode(lights, position, depth, neighbor_offsets[direction]);
        byte light_n1_0 = 0;
        byte light_1_0 = 0;
        byte light_0_n1 = 0;
        byte light_0_1 = 0;
        byte light_n1_n1 = 0;
        byte light_1_1 = 0;
        byte light_n1_1 = 0;
        byte light_1_n1 = 0;
        if (direction == direction_down || direction == direction_up) {
            // Adjacents
            light_n1_0 = getv_nearby_LightNode(lights, position, depth,
                sbyte3_add(neighbor_offsets[direction], (sbyte3) { -1, 0, 0 }));
            light_1_0 = getv_nearby_LightNode(lights, position, depth,
                sbyte3_add(neighbor_offsets[direction], (sbyte3) { 1, 0, 0 }));
            light_0_n1 = getv_nearby_LightNode(lights, position, depth,
                sbyte3_add(neighbor_offsets[direction], (sbyte3) { 0, 0, -1 }));
            light_0_1 = getv_nearby_LightNode(lights, position, depth,
                sbyte3_add(neighbor_offsets[direction], (sbyte3) { 0, 0, 1 }));
            // Corners
            light_n1_n1 = getv_nearby_LightNode(lights, position, depth,
                sbyte3_add(neighbor_offsets[direction], (sbyte3) { -1, 0, -1 }));
            light_1_1 = getv_nearby_LightNode(lights, position, depth,
                sbyte3_add(neighbor_offsets[direction], (sbyte3) { 1, 0, 1 }));
            light_n1_1 = getv_nearby_LightNode(lights, position, depth,
                sbyte3_add(neighbor_offsets[direction], (sbyte3) { -1, 0, 1 }));
            light_1_n1 = getv_nearby_LightNode(lights, position, depth,
                sbyte3_add(neighbor_offsets[direction], (sbyte3) { 1, 0, -1 }));
        } else if (direction == direction_front || direction == direction_back) {
            // Adjacents
            light_n1_0 = getv_nearby_LightNode(lights, position, depth,
                sbyte3_add(neighbor_offsets[direction], (sbyte3) { -1, 0, 0 }));
            light_1_0 = getv_nearby_LightNode(lights, position, depth,
                sbyte3_add(neighbor_offsets[direction], (sbyte3) { 1, 0, 0 }));
            light_0_n1 = getv_nearby_LightNode(lights, position, depth,
                sbyte3_add(neighbor_offsets[direction], (sbyte3) { 0, -1, 0 }));
            light_0_1 = getv_nearby_LightNode(lights, position, depth,
                sbyte3_add(neighbor_offsets[direction], (sbyte3) { 0, 1, 0 }));
            // Corners
            light_n1_n1 = getv_nearby_LightNode(lights, position, depth,
                sbyte3_add(neighbor_offsets[direction], (sbyte3) { -1, -1, 0 }));
            light_1_1 = getv_nearby_LightNode(lights, position, depth,
                sbyte3_add(neighbor_offsets[direction], (sbyte3) { 1, 1, 0 }));
            light_n1_1 = getv_nearby_LightNode(lights, position, depth,
                sbyte3_add(neighbor_offsets[direction], (sbyte3) { -1, 1, 0 }));
            light_1_n1 = getv_nearby_LightNode(lights, position, depth,
                sbyte3_add(neighbor_offsets[direction], (sbyte3) { 1, -1, 0 }));
        } else if (direction == direction_left || direction == direction_right) {
            // Adjacents
            light_n1_0 = getv_nearby_LightNode(lights, position, depth,
                sbyte3_add(neighbor_offsets[direction], (sbyte3) { 0, -1, 0 }));
            light_1_0 = getv_nearby_LightNode(lights, position, depth,
                sbyte3_add(neighbor_offsets[direction], (sbyte3) { 0, 1, 0 }));
            light_0_n1 = getv_nearby_LightNode(lights, position, depth,
                sbyte3_add(neighbor_offsets[direction], (sbyte3) { 0, 0, -1 }));
            light_0_1 = getv_nearby_LightNode(lights, position, depth,
                sbyte3_add(neighbor_offsets[direction], (sbyte3) { 0, 0, 1 }));
            // Corners
            light_n1_n1 = getv_nearby_LightNode(lights, position, depth,
                sbyte3_add(neighbor_offsets[direction], (sbyte3) { 0, -1, -1 }));
            light_1_1 = getv_nearby_LightNode(lights, position, depth,
                sbyte3_add(neighbor_offsets[direction], (sbyte3) { 0, 1, 1 }));
            light_n1_1 = getv_nearby_LightNode(lights, position, depth,
                sbyte3_add(neighbor_offsets[direction], (sbyte3) { 0, -1, 1 }));
            light_1_n1 = getv_nearby_LightNode(lights, position, depth,
                sbyte3_add(neighbor_offsets[direction], (sbyte3) { 0, 1, -1 }));
        }
        for (byte v = 0; v < voxel_face_vertices_length; v++) {
            // Get other lights per vertex
            uint total_light = adjacent_light;
            if (v == 0) {
                // Negative Negative
                total_light += light_0_n1 + light_n1_0 + light_n1_n1;
            } else if (v == 1) {
                // Negative Positive
                total_light += light_0_1 + light_n1_0 + light_n1_1;
            } else if (v == 2) {
                // Positive Positive
                total_light += light_1_0 + light_0_1 + light_1_1;
            } else if (v == 3) {
                // Positive Negative
                total_light += light_0_n1 + light_1_0 + light_1_n1;
            }
            byte light = total_light / 4;
            color_rgb* c = &colors->value[*ccount];
            c->r = light;
            c->g = light;
            c->b = light;
            (*ccount)++;
        }
    }
}

// NOTE: Rebuilds Lights only when MeshColorsGenerate is dirty
zox_sys2(SmoothLightsBuildSystem) {
    byte dbg_log = 0;
    if (!zox_smooth_lighting) {
        return;
    }
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(BuildMesh);
    zox_sys_in(RenderDepth);
    zox_sys_in(MeshColorRGBs);
    zox_sys_out(MeshColorsGenerate);
    zox_sys_out(MeshColorsDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(BuildMesh, build);
        zox_sys_i(RenderDepth, depth);
        zox_sys_i(MeshColorRGBs, colors);
        zox_sys_o(MeshColorsGenerate, generate);
        zox_sys_o(MeshColorsDirty, upload);
        // Dont build when mesh is building
        if (!generate->value || build->value) {
            continue;
        }
        // Get chunk data
        entity chunk = zox_get_parent(world, e);
#ifdef zox_safety_checks
        if (!zox_valid(chunk)) {
            zox_loge("Chunk Parent Invalid for [%s]", zox_getn(e));
            continue;
        }
#endif
        byte chunk_depth = zox_getv(chunk, NodeDepth);
        if (depth->value > chunk_depth) {
            if (dbg_log) {
                zox_log("Chunk Depth is wrong for Light Build [%s]", zox_getn(chunk));
            }
            continue;
        }
        // If chunk building we wait
        if (zox_getv(chunk, BuildChunkSides)) {
            if (dbg_log) {
                zox_log("Chunk [%s] is still Building Sides", zox_get_name(e));
            }
            continue;
        }
        const SidesOctree* sides = zox_get(chunk, SidesOctree);
        const ChunkNeighbors* neighbors = zox_get(chunk, ChunkNeighbors);
        const VoxelNode* voxels = zox_get(chunk, VoxelNode);
        const LightNode* lights = zox_get(chunk, LightNode);
        entity nearby_chunks[27];
        const LightNode* nearby_lights[27];
        fetch_nearby_chunks(world, e, neighbors->value, nearby_chunks);
        fetch_nearby_lights(world, lights, nearby_chunks, nearby_lights);
        uint ccount = 0;
        zox_apply_smooth_lights(nearby_lights, voxels, sides, colors, byte3_zero, &ccount, depth->value, 0);
        generate->value = 0;
        upload->value = 1;
        /*if (ccount > colors->length) {
            zox_logw("Color Verts Missmatch: [%s] Found [%i] Colors [%i]", zox_get_name(e), ccount, colors->length);
        }*/
    }
} zox_sys_end(SmoothLightsBuildSystem);
