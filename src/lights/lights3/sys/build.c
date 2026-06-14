// hmmm issue seems to be about faces
// maybe we redo our mesh builder system

// converts face to octree indexes
static const byte face_children[6][4] = {
    {1, 3, 5, 7}, // +X
    {0, 2, 4, 6}, // -X
    {2, 3, 6, 7}, // +Y
    {0, 1, 4, 5}, // -Y
    {4, 5, 6, 7}, // +Z
    {0, 1, 2, 3}  // -Z
};

static inline const LightNode* get_max_light_on_face(const LightNode* node, byte face, byte depth, byte target) {
    if (!node) {
        return NULL;
    }
    const LightNode* best_node = node;
    byte max_value = node->value;
    if (depth < target && node->ptr) {
        const LightNode* kids = (const LightNode*) node->ptr;
        const byte* idxs = face_children[face];
        for (byte i = 0; i < 4; i++) {
            const LightNode* kid = &kids[idxs[i]];
            const LightNode* candidate = get_max_light_on_face(kid, face, depth + 1, target);
            if (candidate && candidate->value > max_value) {
                max_value = candidate->value;
                best_node = candidate;
            }
        }
    }

    return best_node;
}

static inline const LightNode* get_max_light_on_face_(const LightNode* root_node, const LightNode** nnodes, byte face, byte3 position, byte depth, byte target) {
    const LightNode* node = get_neighbor_LightNode(root_node, nnodes, face, position, depth);
    return get_max_light_on_face(node, face, depth, target);
}

static inline void zox_apply_light3(const LightNode** nnodesl, const VoxelNode* voctree, const SidesOctree* sides, const LightNode* lnode, const MeshColorRGBs* colors, byte3 position, uint* ccount, byte rdepth, byte depth) {
    // Dig Deeper
    if (depth < rdepth && sides->ptr) { // !is_closed_SidesOctree(sides)) {
        const SidesOctree* sides_kids = (const SidesOctree*) sides->ptr;
        // const SidesOctree* sides_kids = get_children_SidesOctree(sides);
        byte has_vkids = !is_closed_VoxelNode(voctree);
        const VoxelNode* vkids = has_vkids ? get_children_VoxelNode(voctree) : NULL;
        byte3_multiply_byte(&position, 2);
        depth++;
        for (byte i = 0; i < 8; i++) {
            /*if (!kids[i].value) {
                continue;
            }*/
            const VoxelNode* cvoctree = has_vkids ? &vkids[i] : voctree;
            byte3 cposition = byte3_add(position, octree_positions_b[i]);
            zox_apply_light3(nnodesl, cvoctree, &sides_kids[i], lnode, colors, cposition, ccount, rdepth, depth);
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
        const LightNode* anode = get_max_light_on_face_(lnode, nnodesl, direction, position, depth, rdepth);
        byte light = anode ? anode->value : 0; // sunlight;
        /*if (zox_disable_low_res_lights && rdepth != terrain_depth) {
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


zox_sys2(Light3BuildSystem) {
    if (disable_lights) {
        return;
    }
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(VoxelNodeDirty);
    zox_sys_in(MeshColorsGenerate);
    zox_sys_in(ChunkNeighbors);
    zox_sys_in(VoxelNode);
    zox_sys_in(SidesOctree);
    zox_sys_in(LightNode);
    zox_sys_in(RenderDepth);
    zox_sys_in(MeshColorRGBs);
    zox_sys_out(MeshColorsDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(VoxelNodeDirty, voxel_octree_dirty);
        zox_sys_i(MeshColorsGenerate, trigger);
        zox_sys_i(ChunkNeighbors, neighbors);
        zox_sys_i(VoxelNode, voxel_octree);
        zox_sys_i(SidesOctree, sides_octree);
        zox_sys_i(LightNode, light_octree);
        zox_sys_i(RenderDepth, render_depth);
        zox_sys_i(MeshColorRGBs, colors);
        zox_sys_o(MeshColorsDirty, mesh_colors_dirty);
        if (trigger->value != zox_dirty_active) {
            continue;
        }
        if (zox_disable_low_res_lights) {
            entity terrain = zox_get_parent(world, e);
            byte terrain_depth = zox_getv(terrain, NodeDepth);
            if (render_depth->value != terrain_depth) {
               continue;
            }
        }
        // Failsafe for when its updating again, no need to double up work
        if (voxel_octree_dirty->value == zox_dirty_trigger || voxel_octree_dirty->value == zox_dirty_active) {
            continue;
        }
        // No Mesh Sides were found
        if (!sides_octree->value) {
            continue;
        }
        const LightNode *nnodesl[6];
        fetch_neightbor_light_nodes(world, neighbors, nnodesl);
        uint ccount = 0;
        zox_apply_light3(nnodesl, voxel_octree, sides_octree, light_octree, colors, byte3_zero, &ccount, render_depth->value, 0);
        if (ccount > colors->length) {
            zox_logw("Color Verts Missmatch: [%s] Found [%i] Colors [%i]", zox_get_name(e), ccount, colors->length);
        }
        mesh_colors_dirty->value = zox_dirty_trigger;
    }
} zox_sys_end(Light3BuildSystem);
