// a wfc node: a new model made from source's patches in a box (model voxels), rules wfc_rule_* (fun/wfc.c)
entity spawn_node_model_wfc(ecs* world, entity source, byte3 box, byte rules) {
    zox_instance(prefab_node_model);
    zox_name("node_model_wfc");
    zox_setv(e, NodeType, zox_model_node_wfc);
    zox_setv(e, Shape3Size, box);
    zox_setv(e, WfcRules, rules);
    zox_set_link(world, e, ModelLink, source);
    return e;
}

// a blueprint of new models like source: each variant its own box (a ratio of source's, the size node) and patches
//  - more nodes (colours, eyes) go between the wfc and the end
entity spawn_model_nodegraph_wfc(ecs* world, entity parent, entity source, byte rules, float3 size_min, float3 size_max) {
    const int3 source_size = zox_getv(source, ChunkSize);
    const byte3 box = int3_to_byte3(source_size);
    entity root_node = spawn_node_model_size(world, size_min, size_max);
    entity wfc_node = spawn_node_model_wfc(world, source, box, rules);
    new_link_single_node(world, root_node, wfc_node);
    entity end_node = spawn_node_model_end(world);
    new_link_single_node(world, wfc_node, end_node);
    zox_set_parent(world, root_node, parent);
    return root_node;
}
