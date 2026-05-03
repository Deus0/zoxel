void spawn_realm_achievements(ecs *world, const entity realm) {
    (void)world;
    (void)realm;
    // zox_muter(realm, AchievementLinks, achievements)
    // clear previous
    // for (int i = 0; i < achievements->length; i++) if (achievements->value[i]) zox_delete(achievements->value[i])
    // initialize_memory_component(AchievementLinks, achievements, entity, 1)
    zox_logv("At [%f] Realm [achievements] [%i] spawned.", zox_current_time, 0);
}
