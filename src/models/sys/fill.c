// Uses a model node to fill with shape data
// DialogueUILink

void process_node_model_fill(ecs* world, entity v, lint seed) {
    if (!zox_valid(v)) {
        return;
    }

    // zox_geter(v, Color, vcolor);
    zox_geter(v, ColorRGBs, colors);
    zox_geter_value(v, NodeDepth, byte, ndepth);
    zox_muter(v, VoxelNode, voctree);
    byte vregions = zox_has(v, VRegions) ? zox_gett_value(v, VRegions) :  16;

    byte2 vrange = (byte2) { 1, colors->length - 1 };
    byte black = colors->length;

    // Run for our fill
    write_lock_VoxelNode(voctree);

    // build_vox_soil(voctree, ndepth, vrange, black, vregions);
    build_vox_bricks(voctree, ndepth, vrange, black);
    if (is_generate_vox_outlines) {
        vox_outlines(voctree, ndepth, black);
    }

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
                process_node_model_fill(world, v, seed->value);
            }
        }  else {
            zox_logw("Node Process Entity does not have ModelLods");
        }

        end->value = zox_dirty_trigger;
    }
} zox_sys_end(FillModelNodeSystem);