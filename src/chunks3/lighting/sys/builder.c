// hmmm issue seems to be about faces
// maybe we redo our mesh builder system

static const byte face_children[6][4] = {
    {1, 3, 5, 7}, // +X
    {0, 2, 4, 6}, // -X
    {2, 3, 6, 7}, // +Y
    {0, 1, 4, 5}, // -Y
    {4, 5, 6, 7}, // +Z
    {0, 1, 2, 3}  // -Z
};

static inline const LightNode* get_max_light_on_face_recursive(
    const LightNode* node,
    byte face,
    byte depth,
    byte target_depth
) {
    if (!node) return NULL;

    const LightNode* best_node = node;
    byte max_value = node->value;

    if (depth < target_depth && node->ptr) {
        const LightNode* kids = (const LightNode*) node->ptr;
        const byte* idxs = face_children[face];
        for (byte i = 0; i < 4; i++) {
            const LightNode* kid = &kids[idxs[i]];
            const LightNode* candidate = get_max_light_on_face_recursive(
                kid, face, depth + 1, target_depth
            );
            if (candidate && candidate->value > max_value) {
                max_value = candidate->value;
                best_node = candidate;
            }
        }
    }

    return best_node;
}

static inline const LightNode* get_max_light_on_face(
    const LightNode* root_node,
    const LightNode** nnodes,
    byte face,
    byte3 position,
    byte depth,
    byte target_depth
) {
    const LightNode* node = get_neighbor_LightNode(
        root_node,
        nnodes,
        face,
        position,
        depth
    );
    return get_max_light_on_face_recursive(node, face, depth, target_depth);
}



void zox_apply_light3(
    const byte* solidity,
    const LightNode** nnodesl,
    const VoxelNode* node,
    const LightNode* root_lnode,
    const MeshColorRGBs* colors,
    byte3 position,
    int* color_index,
    byte target,
    byte depth
) {
    if (!node || node->value == 0) return;

    if (depth >= target || is_closed_VoxelNode(node)) {

        if (node->value && solidity[node->value - 1]) {

            // for each face that is visible according to node->sides
            for (byte face = 0; face < 6; face++) {
                if (*color_index + 4 > colors->length) break;

                // skip hidden face
                if (!(node->sides & (1 << face))) continue;

                // use adjacent lights
                /*const LightNode* adj_node = get_LightNode_neighbor(
                    root_lnode,
                    nnodesl,
                    face,
                    position,
                    depth
                );*/
                const LightNode* adj_node = get_max_light_on_face(
                    root_lnode,
                    nnodesl,
                    face,
                    position,
                    depth,
                    target
                );


                byte light = adj_node ? adj_node->value : 0;
                if (!adj_node) {
                    zox_log_error("Adjacent Node ??? [%ix%ix%i] d[%i]", position.x, position.y, position.z, depth);
                }

                // float factor = light / 255.0f;

                // each face has 4 vertices
                for (int v = 0; v < voxel_face_vertices_length; v++) {
                    color_rgb* c = &colors->value[*color_index];
                    c->r = light;
                    c->g = light;
                    c->b = light;
                    (*color_index)++;
                }
            }
        }
    } else {
        byte3_multiply_byte(&position, 2);
        VoxelNode* kids = get_children_VoxelNode(node);

        for (byte i = 0; i < 8; i++) {
            byte3 positionn = byte3_add(position, octree_positions_b[i]);
            zox_apply_light3(
                solidity,
                nnodesl,
                &kids[i],
                root_lnode,
                colors,
                positionn,
                color_index,
                target,
                depth + 1
            );
        }
    }
}


void Light3BuildSystem(iter* it) {
    zox_ts_begin(light3_builder);
    zox_sys_world();
    zox_sys_begin();

    zox_sys_in(MeshColorsGenerate);
    zox_sys_in(VoxLink);
    zox_sys_in(ChunkNeighbors);
    zox_sys_in(VoxelNode);
    zox_sys_in(LightNode);
    zox_sys_in(RenderDepth);
    zox_sys_in(MeshColorRGBs);
    zox_sys_out(MeshColorsDirty);

    for (int i = 0; i < it->count; i++) {

        zox_sys_i(MeshColorsGenerate, trigger);
        zox_sys_i(VoxLink, vox_link);
        zox_sys_i(ChunkNeighbors, neighbors);
        zox_sys_i(VoxelNode, root_vnode);
        zox_sys_i(LightNode, root_lnode);
        zox_sys_i(RenderDepth, depth);
        zox_sys_i(MeshColorRGBs, colors);
        zox_sys_o(MeshColorsDirty, updated);

        if (trigger->value != zox_dirty_active) {
            continue;
        }

        const LightNode *nnodesl[6];
        fetch_neightbor_light_nodes(
            world,
            neighbors,
            nnodesl);

        zox_geter_value(vox_link->value, RealmLink, entity, realm);
        zox_geter(realm, VoxelLinks, blocks);
        byte solidity[blocks->length];
        for (int j = 0; j < blocks->length; j++) {
            const entity block = blocks->value[j];
            solidity[j] = zox_valid(block) && zox_has(block, BlockModel) ? ((zox_gett_value(block, BlockModel)) == zox_block_solid) : 1;
        }

        int color_index = 0;
        zox_apply_light3(
            solidity,
            nnodesl,
            root_vnode,
            root_lnode,
            colors,
            byte3_zero,
            &color_index,
            depth->value,
            0);

        if (color_index != colors->length) {
            zox_logw("color building not reached max [%i] / [%i]", color_index, colors->length);
        }

        updated->value = zox_dirty_trigger;

        // zox_log("Built Colors [%i] / [%i]", color_index, colors->length);

    }

    zox_ts_end(light3_builder, 1, zox_profile_system_light3_builder);
}
zoxd_system2(Light3BuildSystem);
