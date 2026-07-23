// Uses a model node to fill with shape data
// DialogueUILink
// TODO: Use input nodes for shape and place color
void process_node_model_fill(ecs* world, entity n, entity v, lint seed) {
    if (!zox_valid(n) || !zox_valid(v)) {
        return;
    }
    if (!zox_has(n, NodeDepth) || !zox_has(n, NodeVoxel) || !zox_has(n, Shape3Position) || !zox_has(n, Shape3Size)) {
        zox_logw("Node [%s] has invalid components.", zox_get_name(n));
        return;
    }
    if (!zox_has(v, NodeDepth) || !zox_has(v, ColorRGBs) || !zox_has(v, VoxelNode)) {
        zox_logw("Vox [%s] has invalid components.", zox_get_name(v));
        return;
    }
    // zox_log("Vox [%s] has Valid components.", zox_get_name(v));
    zox_geter_value(n, NodeDepth, byte, ndepth_max);
    zox_geter_value(n, NodeVoxel, byte, fill_type);
    zox_geter_value(n, Shape3Position, byte3, position);
    zox_geter_value(n, Shape3Size, byte3, size);
    zox_geter_value(v, NodeDepth, byte, ndepth);
    // zox_log("Node Depth [%i] - Max [%i]", ndepth, ndepth_max);
    zox_geter(v, ColorRGBs, colors);
    if (!colors->length) {
        zox_log_error("vox [%s] has no colors", zox_get_name(v));
        return;
    }
    if (fill_type > colors->length) {
        zox_log_error("fill type is out of bounds [%i]", fill_type, colors->length);
        fill_type = 1;
    }
    zox_muter(v, VoxelNode, voctree);
    // Change transform for vlength difference
    // zox_log("[%s] OG Transform Data at [%i] [%ix%ix%i] s[%ix%ix%i]", zox_get_name(v), ndepth, position.x, position.y, position.z, size.x, size.y, size.z);
    // NOTE: Scales node sizing to the Vox Size
    short vlength = octree_size(ndepth);
    float max_vlength = (float) powers_of_two[ndepth_max]; // nodegraph_max_depth];
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
    // zox_log("[%s] New Transform Data at [%i] [%ix%ix%i] s[%ix%ix%i]", zox_get_name(v), ndepth, position.x, position.y, position.z, size.x, size.y, size.z);
    // byte2 vrange = (byte2) { 1, colors->length - 1 };
    // byte black = colors->length;
    // Run for our fill
    write_lock_VoxelNode(voctree);
    // zox_log("Filling Cube at [%ix%ix%i] s[%ix%ix%i]", position.x, position.y, position.z, size.x, size.y, size.z);
    // voctree_fill_cube(voctree, ndepth, vrange.x, position, size);
    // voctree_fill_sphere(voctree, ndepth, vrange.x, byte3_single(vlength / 2), vlength / 2);
    voctree_fill_ellipsoid(voctree, ndepth, fill_type, position, size);
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
            for (byte j = 0; j < model_lods_max_length; j++) {
                entity v = models->value[j];
                process_node_model_fill(world, node->value, v, seed->value);
            }
        }  else {
            zox_logw("Node Process Entity does not have ModelLods");
        }
        end->value = zox_dirty_trigger;
    }
} zox_sys_end(FillModelNodeSystem);
