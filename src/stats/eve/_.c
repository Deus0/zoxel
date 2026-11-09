void zox_events_stats(ecs* world) {
    // attributes boost their linked stats
    zox_on_add(
        attribute_bonus_event,
        [none] stats.Stat,
        [none] stats.StatAttribute,
        [none] slots.DataDirty,
    );
}
