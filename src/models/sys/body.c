/*zox_sys2(BodyModelRealmSpawnSystem) {
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
            byte vlength = powers_of_two[block_vox_depth];
            byte3 rsize = (byte3) { vlength / 2 - 1, vlength - 1, vlength / 2 - 1 };
            entity blueprint = spawn_model_nodegraph_slime(world, prefab_node_model);

            // We should make a generic model spawn function here that process uses

            zox_make_neww(model);
            zox_set_unique_name(model, "modelv_slime");
            zox_add_tag(model, ModelCharacter);
            ModelLinks variants = (ModelLinks) { 0 };
            for (int j = 0; j < grass_variants; j++) {
                lint vseed = mseed + j * 1209;

                entity mlods = spawn_model_lods(world, slime_color, vseed, rsize);
                // Create a proocess node and link to model
                // entity process = spawn_process_model(world, prefab_process_model, blueprint, mlods);
                add_to_ModelLinks(&variants, mlods);
            }
            zox_set_ptr(model, ModelLinks, variants);
            add_to_ModelLinks(models, model);
        }

        zox_logv("At [%f] Realm [models] [%i] spawned.", zox_current_time, models->length);
    }
} zox_sys_end(BodyModelRealmSpawnSystem);
*/
