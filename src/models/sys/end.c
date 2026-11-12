// Ends the nodegraph by marking model dirty
void ModelEndNodeSystem(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = zox_dbg_model_nodes;
    zox_sys_world();
    zox_sys_begin();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        entity model = zox_get_link(world, e, ModelLink);
        if (!zox_valid(model)) {
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
        if (node_type != zox_model_node_end) {
            continue;
        }
        if (dbg_log) {
            zox_log("[End] ModelNode  [%s]",
                zox_getn(model));
        }
        entity vox = model;
        if (zox_has(vox, VoxelNode) && zox_has(vox, NodeDepth)) {
            const VoxelNode* voxels = zox_get(vox, VoxelNode);
            byte depth = zox_getv(vox, NodeDepth);
            byte3 size = get_bounds_VoxelNode(voxels, depth);
            zox_setv(vox, ChunkSize, byte3_to_int3(size));
            zox_add(vox, VoxelNodeDirty);
        } else {
            zox_logw("[End] Model [%s] has no VoxelNode", zox_getn(vox));
        }
        zox_add(e, TriggerEnd);
    }
    zox_sys_on_end();
} zoxd_system(ModelEndNodeSystem);
