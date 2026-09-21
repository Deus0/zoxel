// Ends the nodegraph by marking model dirty
zox_sys2(ModelEndNodeSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(NodeBegin);
    zox_sys_in(ModelLink);
    zox_sys_out(NodeEnd);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(NodeBegin, state);
        zox_sys_i(ModelLink, model);
        zox_sys_o(NodeEnd, end);
        if (state->value != zox_dirty_active || !zox_valid(model->value)) {
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
            zox_log(" - Node: Model End [%s]", zox_getn(model->value));
        }
        if (zox_has(model->value, ModelLods)) {
            zox_geter(model->value, ModelLods, models);
            for (byte depth = 0; depth < model_lods_max_length; depth++) {
                entity vox = models->value[depth];
                if (!zox_valid(vox)) {
                    break;
                }
                if (!zox_has(vox, VoxelNode)) {
                    zox_loge("Lod Model [%s] [%i] has Invalid Components", zox_getn(vox), depth);
                    break;
                }
                const VoxelNode* voxels = zox_get(vox, VoxelNode);
                byte3 size = get_octree_bounds(
                    (const void*) voxels,
                    depth,
                    sizeof(VoxelNode),
                    offsetof(VoxelNode, value));
                zox_setv(vox, ChunkSize, byte3_to_int3(size));
                zox_add(vox, VoxelNodeDirty);
            }
        } else {
            zox_logw("Node Process Entity does not have ModelLods");
        }
        end->value = zox_dirty_trigger;
    }
} zox_sys_end(ModelEndNodeSystem);
