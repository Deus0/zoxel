// TODO: Spawn the models in other categories, directly in character/block/gear/body if not shared models
zox_sys2(ModelsRealmSpawnSystem) {
    // byte vlength_max = powers_of_two[nodegraph_max_depth];
    // byte mdepth_vode = block_vox_depth_limits.y;
    byte mdepth_character = block_vox_depth_limits.y;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    zox_sys_out(ModelLinks);
    zox_sys_out(NodegraphLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(GenerateRealm, state);
        zox_sys_o(ModelLinks, models);
        zox_sys_o(NodegraphLinks, graphs);
        if (state->value != zox_generate_realm_models) {
            continue;
        }
        // Spawn Slime
        {
            // TODO: Unique per variant size
            // TODO: Shift Properties from Blueprint to Variant (with seed) - i.e. use Node Process Data
            byte nodegraph_depth = nodegraph_max_depth; // - 1;
            short nodegraph_length = octree_size(nodegraph_depth);
            float squash = randf_range(0.7f, 0.9f);
            float3 ratio = (float3) { squash, 1.0f, squash };
            byte3 vsize_max = byte3_scale3f(byte3_single(nodegraph_length), ratio);
            color skin_color = (color) { rand_range(0, 255), rand_range(0, 255), rand_range(0, 255), 255 };
            color eye_color = (color) { rand_range(0, 255), rand_range(0, 255), rand_range(0, 255), 255 };
            // Create a blueprint for slime
            entity nodegraph = spawn_model_nodegraph_slime(world, prefab_node_model, nodegraph_depth, vsize_max, skin_color, eye_color);
            add_to_NodegraphLinks(graphs, nodegraph);
            // Creates a model group for our variants
            zox_make_neww(model_group);
            zox_set_unique_name(model_group, "modelv_slime");
            zox_add_tag(model_group, ModelCharacter);
            add_to_ModelLinks(models, model_group);
            // We spawn max possible depth models
            ModelLinks variants = (ModelLinks) { 0 };
            lint mseed = 266 * i;
            for (byte j = 0; j < grass_variants; j++) {
                lint vseed = mseed + j * 1209;
                color vcolor = color_grayscale(rand_range(80, 180));
                short vlength = octree_size(mdepth_character);
                byte3 vsize = byte3_scale3f(byte3_single(vlength), ratio);
                ModelLods mlods2 = (ModelLods) { };
                entity mlods = spawn_model_lods(world, vcolor, vseed, mdepth_character, vsize, "rslime", &mlods2);
                zox_set_unique_name(mlods, "mslime");
                add_to_ModelLinks(&variants, mlods);
                spawn_process_model(world, prefab_process_model, nodegraph, mlods);
            }
            zox_set_ptr(model_group, ModelLinks, variants);
        }
        zox_logv("At [%f] Realm [models] [%i] spawned.", zox_current_time, models->length);
    }
} zox_sys_end(ModelsRealmSpawnSystem);

        // Grass
        // TODO: set different heights
        // TODO: Spawn this when spawning Block Grass
        /*{
            color grass_color = color_grayscale(rand_range(50, 120));
            zox_make_neww(e2);
            zox_set_unique_name(e2, "model_group_grass");
            ModelLinks variants = (ModelLinks) { 0 };
            for (int j = 0; j < grass_variants; j++) {
                entity model = spawn_model_grass(world, grass_color, 666 * j, mdepth_vode);
                add_to_ModelLinks(&variants, model);
            }
            zox_set_ptr(e2, ModelLinks, variants);
            add_to_ModelLinks(models, e2);
        }*/
