// Docks arrange canvas elements and reserve the remaining rectangle for the game view.
#include "com/_.c"
#include "fun/_.c"
#include "ins/_.c"
#include "sys/_.c"
#include "eve/_.c"

void import_docks(ecs* world) {
    zox_module(docks);
    zox_components_docks(world);
    zox_events_docks(world);
}
