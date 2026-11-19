/*
 * +------------------------------------------------------+
 * | Zox Module: Consumables (items submodule)            |
 * |                                                      |
 * |  Food - Eating - Bites                               |
 * |                                                      |
 * |  Uses: Items - Stats - Bones - Particles - Sounds    |
 * |                                                      |
 * +------------------------------------------------------+
 *
 *  - a consumable (ItemConsumable, items/com) restores a stat (ConsumeStat) by its ConsumeValue
 *  - used (its press) its user's eat swing starts (sys/consume.c), the hand to the mouth (bones/dat/swings.c)
 *  - at the swing's bites (SwingMark, eve/eat.c): the first eats it (the stat, the stack, its sound), each munches and
 *    drops crumbs in its colours
 */
#include "com/_.c"
#include "ins/_.c"
#include "sys/_.c"
#include "eve/_.c"

void import_consumables(ecs* world) {
    zox_module(consumables);
    zox_components_consumables(world);
    zox_systems_consumables(world);
    zox_events_consumables(world);
}
