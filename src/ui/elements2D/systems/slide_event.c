void SlideEventSystem(iter *it) {
    zox_sys_world()
    zox_sys_begin()
    zox_sys_in(SlideEvent)
    zox_sys_in(SlideBounds)
    zox_sys_in(DraggableState)
    zox_sys_in(DraggerLink)
    zox_sys_in(LayoutConstraints)
    zox_sys_in(LayoutPosition)
    for (int i = 0; i < it->count; i++) {
        zox_sys_e()
        zox_sys_i(SlideEvent, slideEvent)
        zox_sys_i(SlideBounds, slideBounds)
        zox_sys_i(DraggableState, draggableState)
        zox_sys_i(DraggerLink, draggerLink)
        zox_sys_i(LayoutConstraints, limits)
        zox_sys_i(LayoutPosition, pixelPosition)
        if (!draggableState->value || !slideEvent->value) {
            continue;
        }
        int total_width = limits->value.y - limits->value.x;
        // this is between 0 and 1
        float slide_value = - (limits->value.x - pixelPosition->value.x) / (float) total_width;
        // slideBounds is the new bounds
        slide_value = (slide_value * slideBounds->value.y) - slideBounds->value.x;

        // zox_log("value of slider at [%f]", slide_value)
            // todo: calculate based on position within drag b ounds
        const SlideEventData event_data = (SlideEventData) {
            .dragged = e,
            .player = draggerLink->value,
            .value = slide_value,
        };
        (*slideEvent->value)(world, &event_data);
    }
} zoxd_system(SlideEventSystem)