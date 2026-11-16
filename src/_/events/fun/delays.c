// todo: use prefab_event_delay

entity delay_event(
    ecs *world,
    void (*value)(ecs*, entity),
    entity e1,
    double delay)
{
    entity e = zox_new();
    zox_setv(e, TimedEvent, (void*) value);
    zox_setv(e, EventTime, delay);
    zox_setv(e, EventInput, e1);
    return e;
}

entity delay_event2(
    ecs *world,
    void (*value)(ecs*, entity, entity),
    entity e1,
    entity e2,
    double delay)
{
    entity e = zox_new();
    zox_setv(e, TimedEvent, (void*) value);
    zox_setv(e, EventTime, delay);
    zox_setv(e, EventInput, e1);
    zox_setv(e, EventInput2, e2);
    return e;
}
