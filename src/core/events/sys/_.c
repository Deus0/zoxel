#include "timed.c"

void zox_systems_events(ecs* world) {
    zox_system_1(
        TimedEventSystem,
        zoxp_spawn,
        [in] events.TimedEvent,
        [in] events.EventInput,
        [out] events.EventTime,
    );
}
