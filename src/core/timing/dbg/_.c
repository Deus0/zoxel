#include "delta_log.c"
#include "delta.c"
#include "count.c"
#include "time.c"
#include "max.c"
#include "max_data.c"

void add_system_process_counter(ecs* world, entity e) {
    zox_add(e, SystemProcessed);
    zox_add(e, SystemProcessedCache);
}

void define_systems_timing_debug(ecs* world) {
    zox_system(
        SystemProcessedResetSystem,
        zoxp_reset,
        [out] timing.SystemProcessed,
        [out] timing.SystemProcessedCache
    );
    // Gets Curve's Max - TODO: Move to Data module
    zox_system(
        MaxDataSystem,
        zoxp_update,
        [in] core.DoubleData,
        [out] core.MaxDoubleData,
    );
#ifdef zox_time_systems
    // Grabs highest MaxDoubleData from all systems
    zox_system(
        MaxSystemSystem,
        zoxp_update,
        [out] core.SystemLink,
        [none] timing.TrackMaxSystem
    );
    // Logs if SystemDeltaCache is too high
    zox_system_1(
        SystemDeltaLogSystem,
        zoxp_mainthread,
        [in] timing.SystemDeltaCache
    );
    // Sets our SystemDeltaCache (at end of frame)
    zox_system_1(
        SystemDeltaCacheSystem,
        zoxp_end,
        [out] timing.SystemDelta,
        [out] timing.SystemDeltaCache
    );
    // Adds SystemDeltaCache to Curve
    zox_system_1(
        SystemTimeAddSystem,
        zoxp_end,
        [in] timing.SystemDeltaCache,
        [out] core.DoubleData
    );
#endif
}
