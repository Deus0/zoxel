#include "biomes.c"
#include "heights.c"
#include "vegetation.c"
#include "link.c"
#include "end.c"
#include "biome_average.c"
#include "biome_link.c"
#include "region.c"
// Streaming
#include "stream_start.c"
#include "stream_grow.c"
#include "stream_shrink.c"
#include "stream_lod.c"

void define_systems_tunks(ecs* world) {
    zox_system(
        TunkEndSystem,
        EcsOnUpdate,
        [in] core.Generate,
        [in] tunks.Chunk3Stack,
        [none] tunks.Tunk
    );
    zox_system(
        TunkLinkSystem,
        EcsPreUpdate,
        [in] core.Generate,
        [in] chunks2.Chunk2Position,
        [in] voxes.VoxLink,
        [out] tunks.Chunk3Stack,
        [none] tunks.Tunk
    );
    zox_system(
        TunkRegionLinkSystem,
        EcsOnUpdate,
        [in] chunks2.Chunk2Position,
        [out] regions.RegionLink,
        [none] tunks.Tunk
    );
    /*zox_system_1(
        TunkDebugSystem,
        [in] voxes.VoxLink,
        [in] chunks2.Chunk2Position,
        [in] rendering.RenderDistance,
        [out] chunks2.Chunk2Neighbors,
    );*/
    // Move to Biomes
    // NOTE: Generates biome map before height maps
    zox_system(
        BiomeMapSystem,
        EcsOnLoad,
        [in] core.Generate,
        [in] chunks2.Chunk2Position,
        [out] tunks.BiomeMap,
        [none] tunks.Tunk
    );
    zox_system(
        BiomeMapAvgSystem,
        EcsOnUpdate,
        [in] core.Generate,
        [in] voxes.VoxLink,
        [in] tunks.BiomeMap,
        [out] biomes.BiomeLink,
        [none] tunks.Tunk
    );
    zox_system(
        BiomeLinkSystem,
        EcsOnUpdate,
        [in] streaming.StreamPoint2,
        [in] streaming.StreamLink,
        [out] tunks.TunkLink,
        [out] biomes.BiomeLink,
        [none] streaming.Streamer
    );
    // TODO: Move to other modules
    // TODO: Pass in BiomeMap and use biome data
    zox_system(
        HeightMapSystem,
        EcsPreUpdate,
        [in] core.Generate,
        [in] chunks2.Chunk2Position,
        [in] tunks.BiomeMap,
        [out] tunks.HeightMap,
        [none] tunks.Tunk
    );
    // NOTE: Vegetation maps need biomes and temperature maps
    zox_system(
        VegetationMapSystem,
        EcsOnUpdate,
        [in] core.Generate,
        [in] chunks2.Chunk2Position,
        [in] tunks.BiomeMap,
        [out] tunks.VegetationMap,
        [none] tunks.Tunk
    );
    // Streaming!
    zox_system_1(
        FirstTerrainTunkSystem,
        zoxp_mainthread,
        [in] streaming.StreamLink,
        [in] streaming.StreamPoint2,
        [in] streaming.StreamDirty2,
        [none] streaming.Streamer
    );
    zox_filter(
        streamers,
        [in] streaming.StreamerLevel,
        [in] streaming.StreamLink,
        [in] streaming.StreamPoint2,
        [none] streaming.Streamer
    );
    zox_system_ctx_1(
        TunksSpawnSystem,
        zoxp_mainthread,
        streamers,
        [in] chunks2.Chunk2Position,
        [in] rendering.RenderDistance,
        [out] chunks2.Chunk2Neighbors,
        [none] streaming.StreamedChunk,
        [none] tunks.Tunk
    );
    zox_system(
        Tunk2DeathSystem,
        zoxp_destroy,
        [in] voxes.VoxLink,
        [in] chunks2.Chunk2Position,
        [in] rendering.RenderDistance,
        [none] streaming.StreamedChunk,
        [none] tunks.Tunk
    );
    zox_filter(
        streamers_lod,
        [in] streaming.StreamDirty2,
        [in] streaming.StreamerLevel,
        [in] streaming.StreamLink,
        [in] streaming.StreamPoint2,
        [none] streaming.Streamer
    );
    zox_system_ctx(
        TunkLodSystem,
        zoxp_update,
        streamers_lod,
        [in] chunks2.Chunk2Position,
        [out] rendering.RenderDistance,
        [out] rendering.RenderDepth,
        [out] rendering.RenderDistanceDirty,
        [out] rendering.RenderDepthDirty,
        [none] streaming.StreamedChunk
    );
}
