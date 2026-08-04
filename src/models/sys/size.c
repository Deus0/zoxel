// Sets the model size in the nodegraph
zox_sys2(ModelSizeNodeSystem) {
    byte dbg_log = 1;
    ushort inner_seed_shift = 39393;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(NodeBegin);
    zox_sys_in(NodeLink);
    zox_sys_in(ModelLink);
    zox_sys_out(Seed);
    zox_sys_out(ModelSize);
    zox_sys_out(NodeEnd);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(NodeBegin, state);
        zox_sys_i(NodeLink, node);
        zox_sys_i(ModelLink, model);
        zox_sys_o(Seed, seed);
        zox_sys_o(ModelSize, size);
        zox_sys_o(NodeEnd, end);
        if (state->value != zox_dirty_active) {
            continue;
        }
        if (!zox_valid(node->value)) {
            zox_logw("Node in Process is Invalid", zox_getn(e));
            return;
        }
        if (!zox_has(node->value, NodeType)) {
            zox_logw("Node [%s] has no Type.", zox_getn(node->value));
            continue;
        }
        byte node_type = zox_getv(node->value, NodeType);
        if (node_type != zox_model_node_size) {
            continue;
        }
        lint model_seed = zox_getv(model->value, Seed);
        if (!seed->value) {
            seed->value = model_seed;
        }
        // Set our nodegraph size property here!
        // get squash ratio!
        float squash = seed_rangef(seed->value, 0.6f, 1);
        seed->value += inner_seed_shift;
        float stretch = seed_rangef(seed->value, 0.6f, 1);
        seed->value += inner_seed_shift;
        float3 ratio = (float3) { squash, stretch, squash };
        size->value = get_scaled_size(nodegraph_max_depth, ratio);
        end->value = zox_dirty_trigger;
        if (dbg_log) {
            zox_log("New Size of Model [%ix%ix%i]", size->value.x, size->value.y, size->value.z);
        }
    }
} zox_sys_end(ModelSizeNodeSystem);
