//  NOTE: Spawns the food items as children of Realm
zox_sys2(FoodRealmSpawnSystem) {
    // byte dbg_log = 0;
    // zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    for (int i = 0; i < it->count; i++) {
        //zox_sys_e();
        zox_sys_i(GenerateRealm, state);
        if (state->value != zox_generate_realm_items) {
            continue;
        }
        // Spawn a Cookies Item
        {

        }
        // zox_logv("At [%f] Realm [Food] [%i] spawned.", items->length);
    }
} zox_sys_end(FoodRealmSpawnSystem);
