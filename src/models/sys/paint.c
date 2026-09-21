// Uses a model node to fill with shape data
// DialogueUILink

void process_node_model_paint(ecs* world, entity node, entity vox, lint seed, byte3 position, byte3 size) {
    if (!zox_valid(node) || !zox_valid(vox)) {
        return;
    }
    if (!zox_has(node, NodeVoxel) || !zox_has(node, Shape3Position) || !zox_has(node, Shape3Size)) {
        zox_log("Node [%s] has invalid components.", zox_getn(node));
        return;
    }
    if (!zox_has(vox, NodeDepth) || !zox_has(vox, ColorRGBs) || !zox_has(vox, VoxelNode)) {
        zox_log("Vox [%s] has invalid components.", zox_get_name(vox));
        return;
    }
    byte paint_type = zox_getv(node, NodeVoxel);
    byte model_depth = zox_getv(vox, NodeDepth);
    zox_geter(vox, ColorRGBs, colors);
    zox_muter(vox, VoxelNode, voctree);
    if (!paint_type) {
        zox_log_error("Cannot paint air type.");
        return;
    }
    // NOTE: Scales node sizing to the Vox Size
    shift_node_transform(model_depth, &position, &size);
    byte color_index = paint_type - 1;
    if (color_index >= colors->length) {
        zox_loge("Paint Index OOB [%i] colors [%i]", paint_type, colors->length);
        return;
    }
    voctree_paint(voctree, model_depth, paint_type, position, size);
    // zox_log("     - Painting at [%ix%ix%i] s[%ix%ix%i] :: v%i %ix%ix%i", position.x, position.y, position.z, size.x, size.y, size.z, paint_type, vcolor.r, vcolor.g, vcolor.b);
}

// Runs from a Model Node Process
//      This system will simply fill the voxes
zox_sys2(PaintModelNodeSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(NodeBegin);
    zox_sys_in(ModelLink);
    zox_sys_in(ModelSize);
    zox_sys_out(NodeEnd);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(NodeBegin, state);
        zox_sys_i(ModelLink, model);
        zox_sys_i(ModelSize, bounds);
        zox_sys_o(NodeEnd, end);
        if (state->value != zox_dirty_active || !zox_valid(model->value)) {
            continue;
        }
        entity node = zox_get_link(world, e, CurrentNodeLink);
        if (!node) {
            continue;
        }
        zox_geter_value(node, NodeType, byte, ntype);
        if (ntype != zox_model_node_paint) {
            continue;
        }
        // !zox_has(node, NodeDepth) ||
        if (!zox_has(node, NodeVoxel) || !zox_has(node, Shape3Position) || !zox_has(node, Shape3Size)) {
            zox_logw("Node [%s] has invalid components for [zox_model_node_fill].", zox_getn(node));
            continue;
        }
        byte3 position = zox_getv(node, Shape3Position);
        byte3 size = zox_getv(node, Shape3Size);
        if (!byte3_equals(bounds->value, byte3_zero)) {
            scale_node_transform(bounds->value, &position, &size);
        }
        // for each model LOD, run shapes
        zox_logv(" - Node: Model Paint [%s]", zox_get_name(model->value));
        zox_geter(model->value, Seed, seed);
        if (zox_has(model->value, ModelLods)) {
            zox_geter(model->value, ModelLods, models);
            for (int j = 0; j < model_lods_max_length; j++) {
                entity v = models->value[j];
                process_node_model_paint(world, node, v, seed->value, position, size);
            }
        }  else {
            zox_logw("Node Process Entity does not have ModelLods");
        }
        end->value = zox_dirty_trigger;
    }
} zox_sys_end(PaintModelNodeSystem);
