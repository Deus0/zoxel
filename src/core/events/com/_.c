typedef struct {
    void* value;
} TimedEvent;
zoxc_custom(TimedEvent);
zoxc_double(EventTime);
zoxc_entity(EventInput);
zoxc_entity(EventInput2);

// zoxc_function(TimedEvent, void, ecs*, const entity);

void zox_components_events(ecs* world) {
    zoxd(TimedEvent);
    zoxd_double(EventTime);
    zoxd_entity(EventInput);
    zoxd_entity(EventInput2);
}
