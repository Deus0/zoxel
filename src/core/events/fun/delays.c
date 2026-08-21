// todo: use prefab_event_delay

entity delay_event(ecs *world, void (*value)(ecs*, entity), entity t, double delay) {
    entity e = zox_new();
    zox_set(e, TimedEvent, { value });
    zox_set(e, EventInput, { t });
    zox_set(e, EventTime, { delay });
    return e;
}
