// Uses a model node to fill with shape data

void process_node_model_generate_colors(
    ecs* world,
    entity node,
    entity vox,
    lint seed,
    color node_color,
    byte count)
{
    if (!zox_valid(vox) || !zox_valid(node)) {
        return;
    }
    if (!zox_has(vox, ColorRGBs)) {
        zox_loge("[process_node_model_generate_colors] Vox [%s] has invalid components.", zox_get_name(vox));
        return;
    }
    color_rgb vcolor_rgb = color_to_color_rgb(node_color);
    zox_muter(vox, ColorRGBs, colors);
    float2 crange = (float2) {
        1 - default_color_range,
        1 + default_color_range
    };
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
    byte dbg_log = zox_dbg_model_nodes;
    ushort inner_seed_shift = 39393;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ModelLink);
    zox_sys_out(Seed);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ModelLink, model);
        zox_sys_o(Seed, seed);
        if (!zox_valid(model->value)) {
            zox_logw("[ColorsModelNodeSystem] Node has Invalid Model [%s]", zox_getn(e));
            zox_add(e, TriggerEnd);
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
            zox_loge("Node (colors) [%s] has invalid components.",
                zox_getn(node));
            continue;
        }
        // for each model LOD, run shapes
        zox_logv(" - Node: Model Colors [%s]",
            zox_getn(model->value));
        lint model_seed = zox_has(model->value, Seed) ?
            zox_getv(model->value, Seed) : 0;
        if (!seed->value) {
            seed->value = model_seed;
        }
        if (dbg_log) {
            zox_log("[Colors] ModelNode [%s] Seed %i",
                zox_getn(model->value),
                seed);
        }
        color node_color = zox_has(node, Color) ?
            zox_getv(node, Color) :
            seed_color(&seed->value, inner_seed_shift);
        byte color_count = zox_getv(node, NodeColors);
        if (zox_has(model->value, ModelLods)) {
            zox_geter(model->value, ModelLods, models);
            for (int j = 0; j < model_lods_max_length; j++) {
                entity vox = models->value[j];
                if (!zox_valid(vox)) {
                    break;
                }
                process_node_model_generate_colors(
                    world,
                    node,
                    vox,
                    seed->value,
                    node_color,
                    color_count);
                if (dbg_log >= 2) {
                    zox_log("   - [Colors] ModelNode [%s] Depth [%i]",
                        zox_getn(vox),
                        j);
                }
            }
        }  else {
            zox_logw("Node Process Entity does not have ModelLods");
        }
        zox_add(e, TriggerEnd);
    }
} zox_sys_end(ColorsModelNodeSystem);
