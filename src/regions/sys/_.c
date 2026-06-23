#include "stream.c"

int2 region_position_to_block_position2(int2 position, byte terrain_depth) {
    position.x *= region_dividor;
    position.y *= region_dividor;
    byte chunk_length = powers_of_two[terrain_depth];
    position.x *= chunk_length;
    position.y *= chunk_length;
    return position;
}

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
