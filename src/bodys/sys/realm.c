zox_sys2(BodysRealmSpawnSystem) {
    byte nodegraph_vlength = powers_of_two[nodegraph_max_depth];

    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    zox_sys_out(ModelLinks);
    zox_sys_out(ItemLinks);
    zox_sys_out(NodegraphLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(GenerateRealm, state);
        zox_sys_o(ModelLinks, models);
        zox_sys_o(ItemLinks, items);
        zox_sys_o(NodegraphLinks, graphs);

        if (state->value != zox_generate_realm_items) {
            continue;
        }

        {
            byte chest_variants = 1;

            byte3 nodegraph_size = (byte3) { nodegraph_vlength / 2, nodegraph_vlength, nodegraph_vlength / 2 };
            entity nodegraph = spawn_model_nodegraph_chest(world, prefab_node_model, nodegraph_size);
            add_to_NodegraphLinks(graphs, nodegraph);

            zox_make_neww(model_group);
            zox_set_unique_name(model_group, "bodys_chest");
            // zox_add_tag(model_group, ModelBody);
            add_to_ModelLinks(models, model_group);

            byte mdepth = block_vox_depth + 1;

            ModelLinks variants = (ModelLinks) { 0 };
            lint bseed = 888 * i;
            for (byte j = 0; j < chest_variants; j++) {
                lint vseed = bseed + j * 1209;
                color vcolor = (color) { 200, 200, 155, 255 };
                byte vlength = powers_of_two[mdepth];
                byte3 vsize = (byte3) { vlength / 2, vlength, vlength / 2 };

                entity mlods = spawn_model_lods(world, vcolor, vseed, mdepth, vsize, "mchest");
                zox_set_unique_name(mlods, "bodys_chest_mlods");
                add_to_ModelLinks(&variants, mlods);

                entity process = spawn_process_model(world, prefab_process_model, nodegraph, mlods);
            }

            zox_set_ptr(model_group, ModelLinks, variants);

            // entity model = string_hashmap_get(files_hashmap_voxes, new_string_data("playerer"));
            entity model = variants.value[0]; //  model_group;

            // TODO: Generate based on model
            entity texture = string_hashmap_get(files_hashmap_textures, new_string_data("taskbar_body"));

            entity item = spawn_item_body(world, model, texture, "Chest");
            add_to_ItemLinks(items, item);
        }


        zox_logv("At [%f] Realm [bodys] [%i] spawned.", zox_current_time, items->length);
    }
} zox_sys_end(BodysRealmSpawnSystem);
