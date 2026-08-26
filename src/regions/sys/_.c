#include "stream.c"
extern byte dbg_use_new_streaming;

void zox_define_systems_regions(ecs* world) {
    if (!dbg_use_new_streaming) {
        zox_filter(
            streamers,
            [in] streaming.StreamDirty2,
            [in] streaming.StreamPosition2,
            [none] streaming.Streamer
        );
        zox_system_ctx_1(
            RegionSpawnSystem,
            zoxp_spawn,
            streamers,
            [in] core.Seed,
            [in] chunks.NodeDepth,
            [out] regions.RegionLinks
        );
    }
}
