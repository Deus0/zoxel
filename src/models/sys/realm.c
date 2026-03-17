zox_sys2(ModelsRealmSpawnSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    zox_sys_in(Colors);
    zox_sys_out(ModelLinks);
    zox_sys_out(NodegraphLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(GenerateRealm, state);
        zox_sys_i(Colors, colors);
        zox_sys_o(ModelLinks, models);
        zox_sys_o(NodegraphLinks, graphs);

        if (state->value != zox_generate_realm_models) {
            continue;
        }

        // realm data for model gens
        byte vlength_max = powers_of_two[nodegraph_max_depth];

        // get realms colors
        color grass_color;
        if (colors->length < 5 || grayscale_mode) {
            zox_log("+ grayscale_mode enabled")
            grass_color = color_grayscale(144);
        } else {
            grass_color = colors->value[2];
        }

        // models
        // grass - contains a bunch of variants
        // TODO: set different heights
        // TODO: Spawn this when spawning Block Grass
        {
            zox_make_neww(e2);
            zox_set_unique_name(e2, "model_group_grass");

            ModelLinks variants = (ModelLinks) { 0 };
            for (int i = 0; i < grass_variants; i++) {
                entity model = spawn_model_grass(world, grass_color, 666 * i);
                add_to_ModelLinks(&variants, model);
            }
            zox_set_ptr(e2, ModelLinks, variants);

            add_to_ModelLinks(models, e2);
        }

        // spawn slime like npc
        {
            // TODO: Unique per variant size
            byte3 vsize_max = (byte3) { vlength_max - 1, 2 * (vlength_max / 3), vlength_max - 1 };

            // Create a blueprint for slime

            entity nodegraph = spawn_model_nodegraph_slime(world, prefab_node_model, vsize_max);
            add_to_NodegraphLinks(graphs, nodegraph);

            // Creates a model group for our variants
            zox_make_neww(model_group);
            zox_set_unique_name(model_group, "modelv_slime");
            zox_add_tag(model_group, ModelCharacter);
            add_to_ModelLinks(models, model_group);

            byte mdepth = block_vox_depth;

            ModelLinks variants = (ModelLinks) { 0 };
            lint mseed =  266 * i;
            for (byte j = 0; j < grass_variants; j++) {
                lint vseed = mseed + j * 1209;

                // TODO: Shift Properties from Blueprint to Variant (with seed) - i.e. use Node Process Data
                color vcolor = colors->value[3];
                byte vlength = powers_of_two[mdepth];
                byte3 vsize = (byte3) { vlength - 1, 2 * (vlength / 3), vlength - 1 };

                ModelLods mlods2 = (ModelLods) { };
                entity mlods = spawn_model_lods(world, vcolor, vseed, mdepth, vsize, "mslime", &mlods2);
                zox_set_unique_name(mlods, "modelv_lods_slime");
                add_to_ModelLinks(&variants, mlods);

                // Create a proocess node and link to model, which will generate the voxel data
                spawn_process_model(world, prefab_process_model, nodegraph, mlods);
            }

            zox_set_ptr(model_group, ModelLinks, variants);
        }

        // top hat
        {
            // spawn a model
            // then spawn the node tree
            // spawn a node process for every variant we spawn too with different seeds

        }

        zox_logv("At [%f] Realm [models] [%i] spawned.", zox_current_time, models->length);
    }
} zox_sys_end(ModelsRealmSpawnSystem);
