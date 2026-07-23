// TODO: Spawn the models in other categories, directly in character/block/gear/body if not shared models
/*zox_sys2(ModelsRealmSpawnSystem) {
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
        // zox_logv("At [%f] Realm [models] [%i] spawned.", zox_current_time, models->length);
    }
} zox_sys_end(ModelsRealmSpawnSystem);*/

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
