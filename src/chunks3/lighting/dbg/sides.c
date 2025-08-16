// Applies sides to lighting colors to debug

void zox_apply_debug_colors(
    const byte* solidity,
    const VoxelNode* node,
    const MeshColorRGBs* colors,
    int* color_index,
    byte target,
    byte depth
) {
    if (!node || node->value == 0) return;

    static const color_rgb debug_colors[6] = {
        {255,   0,   0}, // +X red
        {  0, 255,   0}, // -X green
        {  0,   0, 255}, // +Y blue
        {255, 255,   0}, // -Y yellow
        {255,   0, 255}, // +Z magenta
        {  0, 255, 255}  // -Z cyan
    };

    if (depth >= target || is_closed_VoxelNode(node)) {
        if (node->value && solidity[node->value - 1]) {
            for (byte face = 0; face < 6; face++) {
                if (*color_index + 4 >= colors->length) break;

                // skip hidden faces
                if (!(node->sides & (1 << face))) {
                    continue;
                }

                // apply debug color to 4 vertices of this face
                for (int v = 0; v < voxel_face_vertices_length; v++) {
                    color_rgb* c = &colors->value[*color_index];
                    *c = debug_colors[face];
                    (*color_index)++;
                }
            }
        }
    } else {
        VoxelNode* kids = get_children_VoxelNode(node);
        for (byte i = 0; i < 8; i++) {
            zox_apply_debug_colors(
                solidity,
                &kids[i],
                colors,
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
        zox_sys_i(VoxelNode, nodev);
        zox_sys_i(LightNode, nodel);
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
        zox_apply_debug_colors(
            solidity,
            nodev,
            colors,
            &color_index,
            depth->value,
            0
        );

        updated->value = zox_dirty_trigger;
    }

    zox_ts_end(light3_builder, 1, zox_profile_system_light3_builder);
}
zoxd_system2(Light3BuildSystem);
