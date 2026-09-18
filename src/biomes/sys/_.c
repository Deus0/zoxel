#include "colors.c"
#include "blocks.c"
#include "maps.c"
#include "link.c"
#include "average.c"

void zox_systems_biomes(ecs *world) {
    zox_system(
        BiomeBlocksDirtySystem,
        zoxp_update,
        [in] core.Generate,
        [none] biomes.Biome
    );
    // Move to Biomes
    // NOTE: Generates biome map before height maps
    zox_system(
        BiomeMapSystem,
        zoxp_update,
        [in] tunks.TunkLod,
        [in] tunks.TunkPosition,
        [out] tunks.GenerateTunk,
        [out] tunks.BiomeMap,
        [none] tunks.Tunk
    );
    zox_system(
        BiomeMapAvgSystem,
        zoxp_update,
        [in] tunks.GenerateTunk,
        [in] tunks.BiomeMap,
        [none] tunks.Tunk
    );
    zox_system(
        BiomeLinkSystem,
        zoxp_update,
        [in] streaming.StreamDirty,
        [in] streaming.StreamPosition,
        [in] streaming.StreamLink,
        [none] streaming.Streamer,
        [none] biomes.BiomeLinker,
    );
}
