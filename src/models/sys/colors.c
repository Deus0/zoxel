// Uses a model node to fill with shape data

void process_node_model_generate_colors(ecs* world, entity v, lint seed) {
    if (!zox_valid(v)) {
        return;
    }

    zox_geter(v, Color, vcolor);
    zox_muter(v, ColorRGBs, colors);
    byte unique_colors = zox_has(v, VoxUniqueColors) ? zox_gett_value(v, VoxUniqueColors) : default_unique_colors;
    float color_range = zox_has(v, VoxColorRange) ? zox_gett_value(v, VoxColorRange) : default_color_range;

    float2 crange = (float2) { 1 - default_color_range, 1 + default_color_range };
    color_rgb vcolor_rgb = color_to_color_rgb(vcolor->value);

    resize_ColorRGBs(colors, 0);

    for (byte i = 0; i < unique_colors; i++) {
        float m = randf_range(crange.x, crange.y);

        color_rgb ucolor = vcolor_rgb;
        color_rgb_multiply_float(&ucolor, m);

        add_to_ColorRGBs(colors, ucolor);
    }

    if (is_generate_vox_outlines) {
        add_to_ColorRGBs(colors, color_rgb_black);
    }
}

// Runs from a Model Node Process
//      This system will simply fill the voxes
zox_sys2(ColorsModelNodeSystem) {
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

        if (ntype != 0) {
            continue;
        }

        // for each model LOD, run shapes

        zox_logv(" - Node: Model Colors [%s]", zox_get_name(model->value));

        zox_geter(model->value, Seed, seed);

        if (zox_has(model->value, ModelLods)) {
            zox_geter(model->value, ModelLods, models);

            for (int j = 0; j < model_lods_max_length; j++) {
                entity v = models->value[j];

                process_node_model_generate_colors(world, v, seed->value);
            }
        }  else {
            zox_logw("Node Process Entity does not have ModelLods");
        }

        end->value = zox_dirty_trigger;
    }
} zox_sys_end(ColorsModelNodeSystem);