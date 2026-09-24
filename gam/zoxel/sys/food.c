//  NOTE: Spawns the food items as children of Realm
void food_generate_system(iter* it) {
    byte dbg_log = 1;
    byte depth = block_depth_limits.y;
    uint seed_shift = 236913;
    byte variants = 1;
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Seed);
    zox_sys_in(GenerateRealm);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Seed, realm_seed);
        zox_sys_i(GenerateRealm, state);
        if (state->value != zox_generate_realm_items) {
            continue;
        }
        uint seed = realm_seed->value + seed_shift;
        entity cookie_nodegraph = spawn_model_nodegraph_cookie(world, e);
        // Spawn a Cookies Item
        {
            entity3 models = spawn_blueprint_models(
                world,
                e,
                cookie_nodegraph,
                seed,
                "cokie",
                depth,
                variants);
            entity model = models.y;
            entity texture_vox = models.z;
            spawn_item_consumable(
                world,
                e,
                "cokie",
                model,
                depth,
                texture_vox,
                direction_front,
                dbg_log);
        }
    }
    zox_sys_on_end();
} zoxd_system(food_generate_system);
