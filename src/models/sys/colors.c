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
    // the complement of the model's first colour (its base): the hue turned round, a seeded nudge to it, and its
    //  brightness the other way, so it always stands out (a cookie's chips on its dough), alien but readable
    if (zox_has(node, HueComplement) && colors->length) {
        const float3 base = color_rgb_to_hsv(colors->value[0]);
        const uint s = (uint) seed * 2246822519u + 3266489917u;
        const float hue = zox_fmod(base.x + 180 + seed_rangef(s, -30, 30) + 360, 360);
        const float saturation = seed_rangef(s + 1, 45, 90);
        const float value = base.z > 50 ? seed_rangef(s + 2, 18, 38) : seed_rangef(s + 2, 70, 92);
        vcolor_rgb = color_to_color_rgb(hsv_to_color((float3) { hue, saturation, value }));
    }
    float2 crange = (float2) {
        1 - default_color_range,
        1 + default_color_range
    };
    // TODO: Reset colors at start of blueprint running
    // resize_ColorRGBs(colors, 0);
    for (byte i = 0; i < count; i++) {
        // shades from the seed, the same model the same colours (was rand(), every run different)
        float m = seed_rangef((uint) seed + i * 7919u + 17u, crange.x, crange.y);
        color_rgb ucolor = vcolor_rgb;
        color_rgb_multiply_float(&ucolor, m);
        add_to_ColorRGBs(colors, ucolor);
    }
}

// Runs from a Model Node Process
//      This system will simply fill the voxes
void colors_model_node_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = zox_dbg_model_nodes;
    ushort inner_seed_shift = 39393;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(Seed);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        entity model = zox_get_link(world, e, ModelLink);
        zox_sys_o(Seed, seed);
        if (!zox_valid(model)) {
            zox_logw("[colors_model_node_system] Node has Invalid Model [%s]", zox_getn(e));
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
            zox_getn(model));
        lint model_seed = zox_has(model, Seed) ?
            zox_getv(model, Seed) : 0;
        if (!seed->value) {
            seed->value = model_seed;
        }
        if (dbg_log) {
            zox_log("[Colors] ModelNode [%s] Seed %i",
                zox_getn(model),
                seed);
        }
        color node_color = zox_has(node, Color) ?
            zox_getv(node, Color) :
            seed_color(&seed->value, inner_seed_shift);
        byte color_count = zox_getv(node, NodeColors);
        process_node_model_generate_colors(
            world,
            node,
            model,
            seed->value,
            node_color,
            color_count);
        zox_add(e, TriggerEnd);
    }
    zox_sys_on_end();
} zoxd_system(colors_model_node_system);
