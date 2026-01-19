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
            zox_make_neww(e2)
            zox_set_unique_name(e2, "model_group_slem");
            zox_add_tag(e2, ModelCharacter)

            ModelLinks variants = (ModelLinks) { 0 };
            for (int i = 0; i < grass_variants; i++) {
                entity model = spawn_model_grass(world,
                    (color) { grass_color.g, grass_color.b, grass_color.r, grass_color.a },
                    2666 * i);
                add_to_ModelLinks(&variants, model);
            }
            zox_set_ptr(e2, ModelLinks, variants)

            add_to_ModelLinks(models, e2);
        }

        // top hat
        {

        }

        zox_logv("At [%f] Realm [models] [%i] spawned.", zox_current_time, models->length);
    }
} zox_sys_end(ModelsRealmSpawnSystem);