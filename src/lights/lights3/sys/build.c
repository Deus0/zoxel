// hmmm issue seems to be about faces
// maybe we redo our mesh builder system

// NOTE: We only use one sub layer now
static inline const LightNode* get_max_light_on_face(const LightNode* node, byte face) {
    if (!node) {
        return NULL;
    }
    const LightNode* best_node = node;
    byte max_value = node->value;
    if (node->ptr) {
        const LightNode* kids = (const LightNode*) node->ptr;
        const byte* indexes = octree_face_children[face];
        for (byte i = 0; i < 4; i++) {
            const LightNode* kid = &kids[indexes[i]];
            // NOTE: Recursively uses highest light on adjacent faces
            //const LightNode* candidate = get_max_light_on_face(kid, face);
            //if (candidate && candidate->value > max_value) {
            //    max_value = candidate->value;
            if (kid->value > max_value) {
                max_value = kid->value;
                best_node = kid;
            }
        }
    }
    return best_node;
}

static inline const LightNode* get_max_light_on_face_start(const LightNode* root_light_octree, const LightNode** neighbor_light_octrees, byte3 position, byte depth, byte face) {
    const LightNode* adjacent_light = get_neighbor_LightNode(root_light_octree, neighbor_light_octrees, face, position, depth);
    return get_max_light_on_face(adjacent_light, face);
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
}

static inline void zox_apply_light3(const LightNode* root_light_octree, const LightNode** neighbor_light_octrees, const VoxelNode* voctree, const SidesOctree* sides, const MeshColorRGBs* colors, byte3 position, uint* ccount, byte render_depth, byte depth) {
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
            zox_apply_light3(root_light_octree, neighbor_light_octrees, cvoctree, &sides_kids[i], colors, child_position, ccount, render_depth, depth);
        }
        return;
    }
    // NOTE: If no sides were rendered
    if (!sides->value) {
        return;
    }
    // for each face that is visible according to node->sides
    for (byte direction = 0; direction < 6; direction++) {
        // skip hidden face
        if (!(sides->value & (1 << (direction + 1)))) {
            continue;
        }
        // TODO: Get Adjacent Depth -> based on chunk index to depth lookup - atm we just assume its render depth + 1
        const LightNode* adjacent_max_light = get_max_light_on_face_start(root_light_octree, neighbor_light_octrees, position, depth, direction);
        byte light = adjacent_max_light ? adjacent_max_light->value : 0;
        // sunlight;
        /*if (zox_disable_low_res_lights && render_depth != terrain_depth) {
            light = sunlight;
        }*/
        // Set lights of our Quads, 4 Verts each
        for (byte v = 0; v < voxel_face_vertices_length; v++) {
            if (*ccount < colors->length) {
                color_rgb* c = &colors->value[*ccount];
                c->r = light;
                c->g = light;
                c->b = light;
            };
            (*ccount)++;
        }
    }
}

// NOTE: Rebuilds Lights only when MeshColorsGenerate is dirty
zox_sys2(Light3BuildSystem) {
    if (disable_lights) {
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
        const LightNode *nnodesl[6];
        fetch_neightbor_light_nodes(world, neighbors, nnodesl);
        uint ccount = 0;
        zox_apply_light3(light_octree, nnodesl, voxel_octree, sides_octree, colors, byte3_zero, &ccount, render_depth->value, 0);
        if (ccount > colors->length) {
            zox_logw("Color Verts Missmatch: [%s] Found [%i] Colors [%i]", zox_get_name(e), ccount, colors->length);
        }
        // NOTE: The same issue appeared here... needed to be synced
        // mesh_colors_dirty->value = zox_dirty_trigger;
        // zox_set(e, MeshDirty, { mesh_state_trigger_terrain });
        // zox_set(e, MeshReady, { 1 });
        ready->value = 1;
    }
} zox_sys_end(Light3BuildSystem);
