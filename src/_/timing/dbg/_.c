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
    // Sets our SystemDeltaCache (at end of frame)
    zox_system(
        SystemDeltaLogResetSystem,
        zoxp_reset,
        [out] timing.SystemDelta,
        [out] timing.SystemDeltaCache
    );
    // Adds SystemDeltaCache to Curve
    zox_system(
        SustemTimePlotSystem,
        zoxp_update - 1,
        [in] timing.SystemDeltaCache,
        [out] core.DoubleData
    );
    // Gets Curve's Max
    zox_system(
        MaxDataSystem,
        zoxp_update,
        [in] core.DoubleData,
        [out] core.MaxDoubleData,
    );
    // Tracks the Max System from their MaxDoubleData
    zox_system(
        MaxSystemSystem,
        zoxp_update + 1,
        [out] core.SystemLink,
        [none] timing.TrackMaxSystem
    );
    zox_system_1(
        SystemDeltaLogSystem,
        zoxp_mainthread,
        [in] timing.SystemDeltaCache
    );
}
