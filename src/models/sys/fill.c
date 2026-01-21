// Uses a model node to fill with shape data
// DialogueUILink

void process_node_model_fill(ecs* world, entity n, entity v, lint seed) {

    if (!zox_valid(v)) {
        return;
    }

    zox_geter_value_non_const(n, Shape3Position, byte3, position);
    zox_geter_value_non_const(n, Shape3Size, byte3, size);

    zox_geter(v, ColorRGBs, colors);
    zox_geter_value(v, NodeDepth, byte, ndepth);
    zox_muter(v, VoxelNode, voctree);
    // byte vregions = zox_has(v, VRegions) ? zox_gett_value(v, VRegions) : 16;
    byte vlength = powers_of_two[ndepth];

    // Change transform for vlength difference

    // zox_log("OG Transform Data at [%i] [%ix%ix%i] s[%ix%ix%i]", ndepth, position.x, position.y, position.z, size.x, size.y, size.z);

    float3 positionf = (float3) { position.x / 32.0f, position.y / 32.0f, position.z / 32.0f };
    position = (byte3) { positionf.x * vlength, positionf.y * vlength, positionf.z * vlength };

    float3 sizef = (float3) { size.x / 32.0f, size.y / 32.0f, size.z / 32.0f };
    size = (byte3) { sizef.x * vlength, sizef.y * vlength, sizef.z * vlength };

    if (size.x == 0) size.x = 1;
    if (size.y == 0) size.y = 1;
    if (size.z == 0) size.z = 1;

    // zox_log("New Transform Data at [%i] [%ix%ix%i] s[%ix%ix%i]", ndepth, position.x, position.y, position.z, size.x, size.y, size.z);

    byte2 vrange = (byte2) { 1, colors->length - 1 };
    byte black = colors->length;

    // Run for our fill
    write_lock_VoxelNode(voctree);
        // zox_log("Filling Cube at [%ix%ix%i] s[%ix%ix%i]", position.x, position.y, position.z, size.x, size.y, size.z);
        // voctree_fill_cube(voctree, ndepth, vrange.x, position, size);
        // voctree_fill_sphere(voctree, ndepth, vrange.x, byte3_single(vlength / 2), vlength / 2);
        voctree_fill_ellipsoid(voctree, ndepth, vrange.x, position, size);

    // build_vox_soil(voctree, ndepth, vrange, black, vregions);
    /*build_vox_bricks(voctree, ndepth, vrange, black);
    if (is_generate_vox_outlines) {
        vox_outlines(voctree, ndepth, black);
    }*/

    write_unlock_VoxelNode(voctree);

    zox_set(v, VoxelNodeDirty, { zox_dirty_trigger });
}

// Runs from a Model Node Process
//      This system will simply fill the voxes
zox_sys2(FillModelNodeSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(NodeBegin);
    zox_sys_in(NodeLink);
    zox_sys_in(ModelLink);
    zox_sys_out(NodeEnd);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(NodeBegin, state);
        zox_sys_i(NodeLink, node);
        zox_sys_i(ModelLink, model);
        zox_sys_o(NodeEnd, end);

        if (state->value != zox_dirty_active || !zox_valid(model->value)) {
            continue;
        }

        zox_geter_value(node->value, NodeType, byte, ntype);

        if (ntype != zox_model_node_fill) {
            continue;
        }

        // for each model LOD, run shapes

        zox_logv(" - Node: Model Fill [%s]", zox_get_name(model->value));

        zox_geter(model->value, Seed, seed);

        if (zox_has(model->value, ModelLods)) {
            zox_geter(model->value, ModelLods, models);

            for (int j = 0; j < model_lods_max_length; j++) {
                entity v = models->value[j];
                process_node_model_fill(world, node->value, v, seed->value);
            }
        }  else {
            zox_logw("Node Process Entity does not have ModelLods");
        }

        end->value = zox_dirty_trigger;
    }
} zox_sys_end(FillModelNodeSystem);