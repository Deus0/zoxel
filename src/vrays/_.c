// VoxelRays into your heart <3
// NOTE: Moved to src/ because needs terrain link now
// TODO: Destroy Gizmo after left game
// TODO: Sometimes gizmos broken on game load now - scales off

#include "set/_.c"
#include "com/_.c"
#include "fun/_.c"
#include "sys/_.c"
#include "dbg/_.c"

void import_vrays(ecs* world) {
    zox_module(vrays);
    zox_components_vrays(world);
    zox_systems_vrays(world);
}
