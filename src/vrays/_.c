// VoxelRays into your heart <3
// NOTE: Moved to src/ because needs terrain link now
#ifndef zoxm_vrays
#define zoxm_vrays

#include "settings/_.c"
#include "components/_.c"
#include "util/_.c"
#include "systems/_.c"
#include "debug/_.c"

zox_begin_module(Vrays)
    define_components_vrays(world);
    define_systems_vrays(world);
zox_end_module(Vrays)

#endif
