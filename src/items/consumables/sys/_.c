#include "consume.c"

void zox_systems_consumables(ecs* world) {
    zox_system_1(
        item_consume_system,
        zoxp_spawn,
        [in] timers.Activate,
        [in] items.Quantity,
        [none] ConsumeValue,
        [none] items.ItemConsumable
    );
}
