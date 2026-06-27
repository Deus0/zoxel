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
        EcsOnLoad,
        [out] timing.SystemProcessed,
        [out] timing.SystemProcessedCache
    );
    zox_system(
        SystemDeltaLogResetSystem,
        EcsOnLoad,
        [out] timing.SystemDelta,
        [out] timing.SystemDeltaCache
    );
    zox_system_1(
        SystemDeltaLogSystem,
        EcsOnStore,
        [in] timing.SystemDeltaCache
    );
    zox_system(
        SustemTimePlotSystem,
        EcsOnLoad,
        [in] timing.SystemDeltaCache,
        [out] core.DataDouble
    );
    zox_system(
        MaxSystemSystem,
        EcsOnUpdate,
        [out] core.SystemLink,
        [none] timing.TrackMaxSystem
    );
    zox_system(
        MaxDataSystem,
        EcsOnUpdate,
        [in] core.DataDouble,
        [out] core.MaxDoubleData,
    );
}
