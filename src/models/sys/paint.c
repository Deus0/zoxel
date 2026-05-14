// Uses a model node to fill with shape data
// DialogueUILink

void process_node_model_paint(ecs* world, entity n, entity v, lint seed) {

    if (!zox_valid(n) || !zox_valid(v)) {
        return;
    }

    if (!zox_has(n, NodeVoxel) || !zox_has(n, Shape3Position) || !zox_has(n, Shape3Size)) {
        zox_log("Node [%s] has invalid components.", zox_get_name(n));
        return;
    }

    if (!zox_has(v, NodeDepth) || !zox_has(v, ColorRGBs) || !zox_has(v, VoxelNode)) {
        zox_log("Vox [%s] has invalid components.", zox_get_name(v));
        return;
    }

    zox_geter_value_non_const(n, NodeVoxel, byte, paint_type);
    zox_geter_value_non_const(n, Shape3Position, byte3, position);
    zox_geter_value_non_const(n, Shape3Size, byte3, size);

    zox_geter_value_non_const(v, NodeDepth, byte, ndepth);
    zox_geter(v, ColorRGBs, colors);
    zox_muter(v, VoxelNode, voctree);

    if (!paint_type) {
        zox_log_error("Cannot paint air type.");
        return;
    }

    // NOTE: Scales node sizing to the Vox Size
    byte vlength = powers_of_two[ndepth];
    float max_vlength = (float) powers_of_two[nodegraph_max_depth]; //  32.0f;
    float3 positionf = (float3) {
        position.x / max_vlength,
        position.y / max_vlength,
        position.z / max_vlength
    };
    position = (byte3) {
        positionf.x * vlength,
        positionf.y * vlength,
        positionf.z * vlength
    };

    float3 sizef = (float3) {
        size.x / max_vlength,
        size.y / max_vlength,
        size.z / max_vlength
    };
    size = (byte3) {
        sizef.x * vlength,
        sizef.y * vlength,
        sizef.z * vlength
    };

    if (size.x == 0) size.x = 1;
    if (size.y == 0) size.y = 1;
    if (size.z == 0) size.z = 1;

    byte color_index = paint_type - 1;
    if (color_index >= colors->length) {
        zox_log_error("Paint Index OOB [%i] colors [%i]", paint_type, colors->length);
        return;
    }

    // color_rgb vcolor = colors->value[color_index];
    // zox_log("     - Painting at [%ix%ix%i] s[%ix%ix%i] :: v%i %ix%ix%i", position.x, position.y, position.z, size.x, size.y, size.z, paint_type, vcolor.r, vcolor.g, vcolor.b);

    // Run for our fill
    write_lock_VoxelNode(voctree);

    voctree_paint(voctree, ndepth, paint_type, position, size);

    write_unlock_VoxelNode(voctree);

    zox_set(v, VoxelNodeDirty, { zox_dirty_trigger });
}

// Runs from a Model Node Process
//      This system will simply fill the voxes
zox_sys2(PaintModelNodeSystem) {
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
        if (ntype != zox_model_node_paint) {
            continue;
        }
        // for each model LOD, run shapes
        zox_logv(" - Node: Model Paint [%s]", zox_get_name(model->value));
        zox_geter(model->value, Seed, seed);
        if (zox_has(model->value, ModelLods)) {
            zox_geter(model->value, ModelLods, models);
            for (int j = 0; j < model_lods_max_length; j++) {
                entity v = models->value[j];
                process_node_model_paint(world, node->value, v, seed->value);
            }
        }  else {
            zox_logw("Node Process Entity does not have ModelLods");
        }
        end->value = zox_dirty_trigger;
    }
} zox_sys_end(PaintModelNodeSystem);
