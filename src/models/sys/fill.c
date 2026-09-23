// Uses a model node to fill with shape data
void process_node_model_fill(ecs* world, entity node, entity vox, lint seed, byte3 position, byte3 size) {
    byte fill_type = zox_getv(node, NodeVoxel);
    // Vox
    byte model_depth = zox_getv(vox, NodeDepth);
    zox_geter(vox, ColorRGBs, colors);
    if (!colors->length) {
        zox_loge("vox [%s] has no colors", zox_getn(vox));
        return;
    }
    if (fill_type > colors->length) {
        zox_loge("fill type is out of bounds [%i]", fill_type, colors->length);
        fill_type = 0; // 1;
    }
    shift_node_transform(model_depth, &position, &size);
    zox_muter(vox, VoxelNode, voctree);
    voctree_fill_ellipsoid(voctree, model_depth, fill_type, position, size);
    // voctree_fill_cube(voctree, ndepth, vrange.x, position, size);
    // voctree_fill_sphere(voctree, ndepth, vrange.x, byte3_single(vlength / 2), vlength / 2);
    // zox_set(vox, VoxelNodeDirty, { zox_dirty_trigger });
    // zox_log("[%s] New Transform Data at [%i] [%ix%ix%i] s[%ix%ix%i]", zox_get_name(v), ndepth, position.x, position.y, position.z, size.x, size.y, size.z);
    // zox_log("Filling Cube at [%ix%ix%i] s[%ix%ix%i]", position.x, position.y, position.z, size.x, size.y, size.z);
    // zox_log("[%s] OG Transform Data at [%i] [%ix%ix%i] s[%ix%ix%i]", zox_get_name(v), ndepth, position.x, position.y, position.z, size.x, size.y, size.z);
}

// Runs from a Model Node Process
//      This system will simply fill the voxes
zox_sys2(FillModelNodeSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ModelLink);
    zox_sys_in(ModelSize);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ModelLink, model);
        zox_sys_i(ModelSize, bounds);
        if (!zox_valid(model->value)) {
            zox_logw("[FillModelNodeSystem] Node has Invalid Model [%s]", zox_getn(e));
            zox_add(e, TriggerEnd);
            continue;
        }
        entity node = zox_get_link(world, e, CurrentNodeLink);
        if (!node) {
            continue;
        }
        if (!zox_valid(node)) {
            zox_logw("Node in Process is Invalid", zox_getn(e));
            return;
        }
        if (!zox_has(node, NodeType)) {
            zox_logw("Node [%s] has no Type.", zox_getn(node));
            continue;
        }
        byte node_type = zox_getv(node, NodeType);
        if (node_type != zox_model_node_fill) {
            continue;
        }
        if (!zox_has(node, NodeVoxel) ||
            !zox_has(node, Shape3Position) ||
            !zox_has(node, Shape3Size))
        {
            zox_loge("[FillModelNodeSystem] Node [%s] has invalid components for [zox_model_node_fill].",
                zox_getn(node));
            continue;
        }
        byte3 position = zox_getv(node, Shape3Position);
        byte3 size = zox_getv(node, Shape3Size);
        if (!byte3_equals(bounds->value, byte3_zero)) {
            scale_node_transform(bounds->value, &position, &size);
        }
        lint seed = zox_has(model->value, Seed) ?
            zox_getv(model->value, Seed) : 0;
        // byte depth = zox_getv(node, NodeDepth);
        // for each model LOD, run shapes
        if (dbg_log) {
            zox_log(" - Node: Model Fill [%s] Seed %i", zox_getn(model->value), seed);
        }
        if (zox_has(model->value, ModelLods)) {
            zox_geter(model->value, ModelLods, models);
            // we should be using max depth here
            for (byte j = 0; j < model_lods_max_length; j++) {
                entity vox = models->value[j];
                if (!zox_valid(vox)) {
                    break;
                }
                if (!zox_has(vox, NodeDepth) || !zox_has(vox, ColorRGBs) || !zox_has(vox, VoxelNode)) {
                    zox_loge("Lod Model [%s] [%i] has Invalid Components", zox_getn(vox), j);
                    break;
                }
                byte model_depth = zox_getv(vox, NodeDepth);
                if (dbg_log) {
                    zox_log("  - Lod Model [%s] Depth [%i]", zox_getn(vox), model_depth);
                }
                process_node_model_fill(world, node, vox, seed, position, size);
            }
        }  else {
            zox_logw("Node Process Entity does not have ModelLods");
        }
        zox_add(e, TriggerEnd);
    }
} zox_sys_end(FillModelNodeSystem);
