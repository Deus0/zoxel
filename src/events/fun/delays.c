// todo: use prefab_event_delay

entity delay_event(ecs *world, void (*value)(ecs*, const entity), const entity e, const double delay) {
    zox_make_neww(event)
    zox_set(event, TimedEvent, { value })
    zox_set(event, EventInput, { e })
    zox_set(event, EventTime, { delay })
    return event;
}
