void spawn_realm_models(ecs *world, const entity e) {
    if (!zox_valid(e) || !zox_has(e, ModelLinks)) {
        return;
    }

    zox_geter(e, ModelLinks, old);
    if (old) {
        for (int i = 0; i < old->length; i++) {
            zox_delete(old->value[i])
        }
    }

    // get realms colors
    zox_geter(e, Colors, realm_colors);
    color grass_color;
    if (realm_colors->length < 5 || grayscale_mode) {
        zox_log("+ grayscale_mode enabled")
        grass_color = color_grayscale(144);
    } else {
        grass_color = realm_colors->value[2];
    }

    ModelLinks models = (ModelLinks) { 0, NULL };

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

        add_to_ModelLinks(&models, e2);
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

        add_to_ModelLinks(&models, e2);
    }

    // top hat

    zox_set_ptr(e, ModelLinks, models);

    zox_logv("At [%f] Realm [models] [%i] spawned.", zox_current_time, models.length);
}