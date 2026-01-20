zox_sys2(ModelsRealmSpawnSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    zox_sys_in(Colors);
    zox_sys_out(ModelLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(GenerateRealm, state);
        zox_sys_i(Colors, colors);
        zox_sys_o(ModelLinks, models);

        if (state->value != zox_generate_realm_models) {
            continue;
        }

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
        // todo: set different heights
        {
            zox_make_neww(e2)
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
            lint slime_seed =  266 * i;
            color slime_color = colors->value[3];

            // Create a blueprint for slime
            entity slime_node = spawn_model_nodegraph_slime(world, prefab_node_model);

            // We should make a generic model spawn function here that process uses

            zox_make_neww(e2)
            zox_set_unique_name(e2, "modelv_slime");
            zox_add_tag(e2, ModelCharacter);

            ModelLinks variants = (ModelLinks) { 0 };

            for (int j = 0; j < grass_variants; j++) {
                lint vseed = slime_seed + j * 1209;

                entity model = spawn_model_lods(world, slime_color, vseed);

                // Create a proocess node and link to model
                entity process = spawn_process_model(world, prefab_process_model, slime_node, model);


                add_to_ModelLinks(&variants, model);
            }

            zox_set_ptr(e2, ModelLinks, variants);

            add_to_ModelLinks(models, e2);
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