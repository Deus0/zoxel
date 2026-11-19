// a mutate node: its process's model's colours mutated by the model's seed (mutate_color), ColorMutation its range
void model_mutate_node_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        const entity node = zox_get_link(world, e, CurrentNodeLink);
        if (!zox_valid(node) || !zox_has(node, NodeType) || zox_getv(node, NodeType) != zox_model_node_mutate) {
            continue;
        }
        const entity model = zox_get_link(world, e, ModelLink);
        if (zox_valid(model) && zox_has(model, ColorRGBs)) {
            const float3 range = zox_getv(node, ColorMutation);
            const uint seed = seed_rand((uint) zox_getv(model, Seed) + 7177);
            const float turn = seed_rangef(seed, -range.x, range.x);
            const float tint = seed_rangef(seed + 1, 0, 360);
            zox_muter(model, ColorRGBs, colors);
            for (int j = 0; j < colors->length; j++) {
                colors->value[j] = mutate_color(colors->value[j], seed + 2 + j * 7919u, turn, tint, range);
            }
        }
        zox_add(e, TriggerEnd);
    }
}
