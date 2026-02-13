zox_sys2(SlideEventSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SlideEvent);
    zox_sys_in(SlideBounds);
    zox_sys_in(DraggableState);
    zox_sys_in(DraggerLink);
    zox_sys_in(LayoutConstraints);
    zox_sys_in(LayoutPosition);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SlideEvent, slideEvent);
        zox_sys_i(SlideBounds, bounds);
        zox_sys_i(DraggableState, draggableState);
        zox_sys_i(DraggerLink, draggerLink);
        zox_sys_i(LayoutConstraints, limits);
        zox_sys_i(LayoutPosition, pixelPosition);

        if (!draggableState->value || !slideEvent->value) {
            continue;
        }

        int total_width = limits->value.y - limits->value.x;
        // this is between 0 and 1
        float percent = - (limits->value.x - pixelPosition->value.x) / (float) total_width;
        // slideBounds is the new bounds
        float slide_value = bounds->value.x  + (percent * (bounds->value.y - bounds->value.x));

        zox_log("value of slider at [%f] [%f]", percent, slide_value);

        // todo: calculate based on position within drag bounds

        SlideEventData event_data = (SlideEventData) {
            .dragged = e,
            .player = draggerLink->value,
            .value = slide_value,
        };

        (*slideEvent->value)(world, &event_data);
    }
} zox_sys_end(SlideEventSystem);
