// a wfc node: a new model made from source's patches in a box (model voxels), rules wfc_rule_* (fun/wfc.c)
entity spawn_node_model_wfc(ecs* world, entity source, byte3 box, ushort rules) {
    zox_instance(prefab_node_model);
    zox_name("node_model_wfc");
    zox_setv(e, NodeType, zox_model_node_wfc);
    zox_setv(e, Shape3Size, box);
    zox_setv(e, WfcRules, rules);
    zox_set_link(world, e, ModelLink, source);
    return e;
}

// a mutate node: the model's colours mutated by its seed, range: hue turn (degrees), saturation added, value change
entity spawn_node_model_mutate(ecs* world, float3 range) {
    zox_instance(prefab_node_model);
    zox_name("node_model_mutate");
    zox_setv(e, NodeType, zox_model_node_mutate);
    zox_setv(e, ColorMutation, range);
    return e;
}

// a blueprint of new models like source: each variant its own box (a ratio of source's, the size node) and patches
//  - more nodes (colours, eyes) go between the wfc and the end
//  - mutation: its colours' (spawn_node_model_mutate), none if zero
entity spawn_model_nodegraph_wfc(ecs* world, entity parent, entity source, ushort rules, float3 size_min, float3 size_max,
    float3 mutation)
{
    const int3 source_size = zox_getv(source, ChunkSize);
    const byte3 box = int3_to_byte3(source_size);
    entity root_node = spawn_node_model_size(world, size_min, size_max);
    entity wfc_node = spawn_node_model_wfc(world, source, box, rules);
    new_link_single_node(world, root_node, wfc_node);
    entity last = wfc_node;
    if (mutation.x || mutation.y || mutation.z) {
        const entity mutate_node = spawn_node_model_mutate(world, mutation);
        new_link_single_node(world, last, mutate_node);
        last = mutate_node;
    }
    entity end_node = spawn_node_model_end(world);
    new_link_single_node(world, last, end_node);
    zox_set_parent(world, root_node, parent);
    return root_node;
}
