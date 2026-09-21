// Sets the model size in the nodegraph
zox_sys2(ModelSizeNodeSystem) {
    byte dbg_log = 0;
    ushort inner_seed_shift = 39393;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(NodeBegin);
    zox_sys_in(ModelLink);
    zox_sys_out(Seed);
    zox_sys_out(ModelSize);
    zox_sys_out(NodeEnd);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(NodeBegin, state);
        zox_sys_i(ModelLink, model);
        zox_sys_o(Seed, seed);
        zox_sys_o(ModelSize, size);
        zox_sys_o(NodeEnd, end);
        if (state->value != zox_dirty_active) {
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
        if (node_type != zox_model_node_size) {
            continue;
        }
        if (!zox_has(node, ModelRatio)) {
            zox_loge("Node has no ModelRatio");
            continue;
        }
        lint model_seed = zox_getv(model->value, Seed);
        if (!seed->value) {
            seed->value = model_seed;
        }
        float6 limits = zox_getv(node, ModelRatio);
        // Set our nodegraph size property here!
        // get squash ratio!
        float x = seed_rangef(seed->value, limits.x, limits.y); // 0.6f, 1);
        seed->value += inner_seed_shift;
        float y = seed_rangef(seed->value, limits.z, limits.w); // 0.6f, 1);
        seed->value += inner_seed_shift;
        float z = seed_rangef(seed->value, limits.u, limits.v);
        seed->value += inner_seed_shift;
        float3 ratio = (float3) { x, y, z };
        size->value = get_scaled_size(nodegraph_max_depth, ratio);
        end->value = zox_dirty_trigger;
        if (dbg_log) {
            zox_log("New Size of Model [%ix%ix%i]", size->value.x, size->value.y, size->value.z);
        }
    }
} zox_sys_end(ModelSizeNodeSystem);
