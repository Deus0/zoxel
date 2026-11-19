// Add observer for QuantityDirty
#include "quantity_zero.c"
#include "place.c"

void zox_events_items(ecs* world) {
    zox_on_add(
        quantity_zero_event,
        [in] items.Quantity,
        [none] items.Item,
        [none] items.QuantityDirty,
    );
    zox_observer(on_place_hit, EcsOnSet, [in] bones.SwingMark, [none] bones.Skeleton);
}