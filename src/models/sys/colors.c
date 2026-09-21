// Uses a model node to fill with shape data

void process_node_model_generate_colors(ecs* world, entity n, entity v, lint seed, color node_color, byte count) {
    if (!zox_valid(v) || !zox_valid(n)) {
        return;
    }
    if (!zox_has(v, ColorRGBs)) {
        zox_loge("Vox [%s] has invalid components.", zox_get_name(v));
        return;
    }
    color_rgb vcolor_rgb = color_to_color_rgb(node_color);
    zox_muter(v, ColorRGBs, colors);
    float2 crange = (float2) { 1 - default_color_range, 1 + default_color_range };
    // TODO: Reset colors at start of blueprint running
    // resize_ColorRGBs(colors, 0);
    for (byte i = 0; i < count; i++) {
        float m = randf_range(crange.x, crange.y);
        color_rgb ucolor = vcolor_rgb;
        color_rgb_multiply_float(&ucolor, m);
        add_to_ColorRGBs(colors, ucolor);
    }
}

// Runs from a Model Node Process
//      This system will simply fill the voxes
zox_sys2(ColorsModelNodeSystem) {
    ushort inner_seed_shift = 39393;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(NodeBegin);
    zox_sys_in(ModelLink);
    zox_sys_out(Seed);
    zox_sys_out(NodeEnd);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(NodeBegin, state);
        zox_sys_i(ModelLink, model);
        zox_sys_o(Seed, seed);
        zox_sys_o(NodeEnd, end);
        if (state->value != zox_dirty_active || !zox_valid(model->value)) {
            continue;
        }
        entity node = zox_get_link(world, e, CurrentNodeLink);
        if (!node) {
            continue;
        }
        byte node_type = zox_getv(node, NodeType);
        if (node_type != zox_model_node_colors) {
            continue;
        }
        if (!zox_has(node, NodeColors)) {
            zox_loge("Node (colors) [%s] has invalid components.", zox_getn(node));
            continue;
        }
        // for each model LOD, run shapes
        zox_logv(" - Node: Model Colors [%s]", zox_getn(model->value));
        lint model_seed = zox_getv(model->value, Seed);
        if (!seed->value) {
            seed->value = model_seed;
        }
        // color node_color = zox_getv(node->value, Color);
        color node_color = zox_has(node, Color) ? zox_getv(node, Color) : seed_color(&seed->value, inner_seed_shift);
        byte color_count = zox_getv(node, NodeColors);
        if (zox_has(model->value, ModelLods)) {
            zox_geter(model->value, ModelLods, models);
            for (int j = 0; j < model_lods_max_length; j++) {
                entity vox = models->value[j];
                process_node_model_generate_colors(
                    world,
                    node,
                    vox,
                    seed->value,
                    node_color,
                    color_count);
            }
        }  else {
            zox_logw("Node Process Entity does not have ModelLods");
        }
        end->value = zox_dirty_trigger;
    }
} zox_sys_end(ColorsModelNodeSystem);
