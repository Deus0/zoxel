#include "destroy_in_frame.c"
#include "event_debug.c"
#include "timed_event.c"

void define_systems_generic(ecs* world) {
    zox_system(
        DestroyInFrameSystem,
        EcsOnStore,
        [none] DestroyInFrame
    );
    zox_system_1(
        TimedEventSystem,
        EcsPreStore,
        [in] TimedEvent,
        [in] generic.EventInput,
        [out] EventTime
    );
}