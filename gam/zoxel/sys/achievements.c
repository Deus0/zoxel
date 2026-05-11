zox_sys2(AchievementRealmSpawnSystem) {
    // zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    zox_sys_out(AchievementLinks);
    for (int i = 0; i < it->count; i++) {
        // zox_sys_e();
        zox_sys_i(GenerateRealm, state);
        zox_sys_o(AchievementLinks, achievements);
        if (state->value != zox_generate_realm_achievements) {
            continue;
        }
        // TODO: This.
        zox_logv("At [%f] Realm [achievements] [%i] spawned.", zox_current_time, achievements->length);
    }
} zox_sys_end(AchievementRealmSpawnSystem);
