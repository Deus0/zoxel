#include "stream.c"

void zox_define_systems_regions(ecs* world) {
    zox_filter(
        streamers,
        [in] streaming.StreamDirty2,
        [in] streaming.StreamPosition2,
        [none] streaming.Streamer
    );
    zox_system_ctx_1(
        RegionSpawnSystem,
        zoxp_mainthread,
        streamers,
        [in] core.Seed,
        [out] regions.RegionLinks
    );
}
