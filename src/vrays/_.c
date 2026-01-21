// VoxelRays into your heart <3
// NOTE: Moved to src/ because needs terrain link now
#ifndef zoxm_vrays
#define zoxm_vrays

// TODO: Destroy Gizmo after left game
// TODO: Sometimes gizmos broken on game load now - scales off

#include "set/_.c"
#include "com/_.c"
#include "fun/_.c"
#include "sys/_.c"
#include "dbg/_.c"

zox_begin_module(Vrays)
    define_components_vrays(world);
    define_systems_vrays(world);
zox_end_module(Vrays)

#endif
