// hmmm issue seems to be about faces
// maybe we redo our mesh builder system

static inline void zox_apply_smooth_lights(const LightNode** lights, const VoxelNode* voctree, const SidesOctree* sides, const MeshColorRGBs* colors, byte3 position, uint* ccount, byte render_depth, byte depth) {
    // Dig Deeper
    if (depth < render_depth && sides->ptr) {
        const SidesOctree* sides_kids = (const SidesOctree*) sides->ptr;
        byte has_vkids = !is_closed_VoxelNode(voctree);
        const VoxelNode* vkids = has_vkids ? get_children_VoxelNode(voctree) : NULL;
        byte3_multiply_byte(&position, 2);
        depth++;
        for (byte i = 0; i < 8; i++) {
            const VoxelNode* cvoctree = has_vkids ? &vkids[i] : voctree;
            byte3 child_position = byte3_add(position, octree_positions_b[i]);
            zox_apply_smooth_lights(lights, cvoctree, &sides_kids[i], colors, child_position, ccount, render_depth, depth);
            if (*ccount >= colors->length) {
                break;
            }
        }
        return;
    }
    // NOTE: If no sides were rendered
    if (!sides->value) {
        return;
    }
    // TODO: If two adjacent voxels solid, dont use corner light!
    // for each face that is visible according to node->sides
    // byte** lights = ..
    for (byte direction = 0; direction < 6; direction++) {
        // skip hidden face
        if (!(sides->value & (1 << (direction + 1)))) {
            continue;
        }
        // TODO: Get Adjacent Depth -> based on chunk index to depth lookup - atm we just assume its render depth + 1
        // const LightNode* adjacent_light = get_neighbor_LightNode(lights, neighbor_lights, direction, position, depth);
        // NOTE: We are basing this off the verts mesh.c voxel_face_vertices_n
        // Else if smooth lighting, each point gets different lights
        // Complicated because lights need to be the ones touching the vertex, this algorithm didn't account for corners
        // First direct adjacent tops 2
        // int3 positioni = byte3_to_int3(position);
        byte adjacent_light = getv_nearby_LightNode(lights, position, depth,  neighbor_offsets[direction]);
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
            /*for (byte v = 0; v < voxel_face_vertices_length; v++) {
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
                if (*ccount >= colors->length) {
                    return;
                }
            }*/
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
            /*for (byte v = 0; v < voxel_face_vertices_length; v++) {
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
                if (*ccount >= colors->length) {
                    return;
                }
            }*/
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
            if (*ccount >= colors->length) {
                return;
            }
        }
    }
}

// NOTE: Rebuilds Lights only when MeshColorsGenerate is dirty
zox_sys2(SmoothLightsBuildSystem) {
    if (disable_lights) {
        return;
    }
    if (!zox_smooth_lighting) {
        return;
    }
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(MeshColorsGenerate);
    zox_sys_in(ChunkNeighbors);
    zox_sys_in(VoxelNode);
    zox_sys_in(SidesOctree);
    zox_sys_in(LightNode);
    zox_sys_in(RenderDepth);
    zox_sys_in(MeshColorRGBs);
    zox_sys_out(MeshReady);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(MeshColorsGenerate, trigger);
        zox_sys_i(ChunkNeighbors, neighbors);
        zox_sys_i(VoxelNode, voxel_octree);
        zox_sys_i(SidesOctree, sides_octree);
        zox_sys_i(LightNode, light_octree);
        zox_sys_i(RenderDepth, render_depth);
        zox_sys_i(MeshColorRGBs, colors);
        zox_sys_o(MeshReady, ready);
        if (trigger->value != zox_dirty_active) {
            continue;
        }
        if (zox_getv(e, BuildChunkMesh)) {
            // if still building, it will update the verts again
            zox_set(e, MeshColorsGenerate, { zox_dirty_trigger });
            if (dbg_log) {
                zox_log("Chunk is still Generating new Mesh, while building Lights [%s]", zox_get_name(e));
            }
            continue;
        }
        if (zox_disable_low_res_lights) {
            entity terrain = zox_get_parent(world, e);
            byte terrain_depth = zox_getv(terrain, NodeDepth);
            if (render_depth->value != terrain_depth) {
               continue;
            }
        }
        // No Mesh Sides were found
        if (!sides_octree->value) {
            continue;
        }
        entity nearby_chunks[27];
        const LightNode* nearby_lights[27];
        fetch_nearby_chunks(world, e, neighbors->value, nearby_chunks);
        fetch_nearby_lights(world, light_octree, nearby_chunks, nearby_lights);
        //const LightNode *nnodesl[6];
        //fetch_neightbor_light_nodes(world, neighbors, nnodesl);
        uint ccount = 0;
        zox_apply_smooth_lights(nearby_lights, voxel_octree, sides_octree, colors, byte3_zero, &ccount, render_depth->value, 0);
        if (ccount > colors->length) {
            zox_logw("Color Verts Missmatch: [%s] Found [%i] Colors [%i]", zox_get_name(e), ccount, colors->length);
        }
        ready->value = 1;
    }
} zox_sys_end(SmoothLightsBuildSystem);

    /*if (!adjacent_light) {
        return NULL;
    }
    byte max_value = adjacent_light->value;
    const LightNode* max_light = adjacent_light;
    if (adjacent_light->ptr) {
        const LightNode* children = (const LightNode*) adjacent_light->ptr;
        for (int i = 0; i < 8; i++) {
            const LightNode* kid = &children[i];
            if (kid->value > max_value) {
                max_value = kid->value;
                max_light = kid;
            }
        }
    }
    return max_light;*/
