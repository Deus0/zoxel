#include "delta_log.c"
#include "delta.c"
#include "count.c"
#include "time.c"
#include "max_system.c"
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
        [out] core.DoubleDataMax,
    );
#ifdef zox_time_systems
    // Sets our SystemDeltaCache (at end of frame)
    zox_system_1(
        SystemDeltaCacheSystem,
        zoxp_dbg_end,
        [out] timing.SystemDelta,
        [out] timing.SystemDeltaCache
    );
    // Adds SystemDeltaCache to Curve
    zox_system_1(
        SystemTimeAddSystem,
        zoxp_dbg_begin,
        [in] timing.SystemDeltaCache,
        [out] core.DoubleData
    );
    // Logs if SystemDeltaCache is too high
    zox_system_1(
        SystemDeltaLogSystem,
        zoxp_dbg_begin,
        [in] timing.SystemDeltaCache
    );
    // Grabs highest DoubleDataMax from all systems
    zox_system(
        MaxSystemSystem,
        zoxp_update,
        [in] core.DoubleDataMax,
        [none] core.ZoxSystem
    );
#endif
}
