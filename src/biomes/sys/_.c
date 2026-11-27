#include "colors.c"
#include "blocks.c"
#include "maps.c"
#include "link.c"
#include "average.c"

void zox_systems_biomes(ecs *world) {
    zox_system(
        biome_blocks_dirty_system,
        zoxp_update,
        [in] core.Generate,
        [none] biomes.Biome
    );
    // Move to Biomes
    // NOTE: Generates biome map before height maps
    zox_system(
        biome_map_system,
        zoxp_update,
        [in] tunks.TunkLod,
        [in] tunks.TunkPosition,
        [out] tunks.GenerateTunk,
        [out] tunks.BiomeMap,
        [none] tunks.Tunk
    );
    zox_system(
        biome_map_avg_system,
        zoxp_update,
        [in] tunks.GenerateTunk,
        [in] tunks.BiomeMap,
        [none] tunks.Tunk
    );
    // multi threaded, after the tunks' biomes are linked: the sky's blend (a tween, an entity) started by its event
    //  (SkyBlendStart, weathers' observer on the main thread at the merge), so no main thread system
    zox_system(
        biome_link_system,
        zoxp_pre_end,
        [in] streaming.StreamDirty,
        [in] streaming.StreamPosition,
        [none] streaming.Streamer,
        [none] biomes.BiomeLinker,
    );
}
