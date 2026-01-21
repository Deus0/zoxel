zox_sys2(BodyModelRealmSpawnSystem) {
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
        color skincolor = colors->value[2];

        // TODO: Spawn Chest here after we finish the Slime node process properties architecture!

        // Chest
        {
            lint mseed = 266 * i;
            color slime_color = colors->value[3];

            // Create a blueprint for slime
            byte3 rsize = (byte3) { 30, 16, 30 };
            entity blueprint = spawn_model_nodegraph_slime(world, prefab_node_model);

            // We should make a generic model spawn function here that process uses

            zox_make_neww(e2)
            zox_set_unique_name(e2, "modelv_slime");
            zox_add_tag(e2, ModelCharacter);

            ModelLinks variants = (ModelLinks) { 0 };

            for (int j = 0; j < grass_variants; j++) {
                lint vseed = mseed + j * 1209;

                entity model = spawn_model_lods(world, slime_color, vseed, rsize);

                // Create a proocess node and link to model
                // entity process = spawn_process_model(world, prefab_process_model, blueprint, model);


                add_to_ModelLinks(&variants, model);
            }

            zox_set_ptr(e2, ModelLinks, variants);

            add_to_ModelLinks(models, e2);
        }

        zox_logv("At [%f] Realm [models] [%i] spawned.", zox_current_time, models->length);
    }
} zox_sys_end(BodyModelRealmSpawnSystem);