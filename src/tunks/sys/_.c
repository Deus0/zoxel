// Link
#include "region_link.c"
// Streaming
#include "stream_start.c"
#include "stream_grow.c"
#include "stream_shrink.c"
#include "stream_lod.c"
// Stack
#include "stack_link.c"
#include "stack_trigger.c"
// Biomes
#include "biome_link.c"
#include "biomes.c"
#include "biome_average.c"
// More Maps
#include "heights.c"
#include "texture.c"
#include "texture_heights.c"
extern byte dbg_use_new_streaming;

void define_systems_tunks(ecs* world) {
    zox_system(
        TunkEndSystem,
        zoxp_update,
        [in] tunks.Chunk3Stack,
        [out] tunks.GenerateTunk,
        [none] tunks.Tunk
    );
    /*zox_system_1(
        TunkDebugSystem,
        [in] tunks.TunkPosition,
        [in] rendering.RenderDistance,
        [out] chunks2.Chunk2Neighbors,
    );*/
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
        [out] biomes.BiomeLink,
        [none] tunks.Tunk
    );
    zox_system(
        BiomeLinkSystem,
        zoxp_update,
        [in] streaming.StreamDirty2,
        [in] streaming.StreamPosition2,
        [in] streaming.StreamLink,
        [out] tunks.TunkLink,
        [out] biomes.BiomeLink,
        [none] streaming.Streamer
    );
    // TODO: Move to other modules
    // TODO: Pass in BiomeMap and use biome data
    zox_system(
        HeightMapSystem,
        zoxp_update,
        [in] tunks.TunkLod,
        [in] tunks.TunkPosition,
        [in] tunks.BiomeMap,
        [out] tunks.GenerateTunk,
        [out] tunks.HeightMap,
        [none] tunks.Tunk
    );
    zox_system(
        Tunk2DeathSystem,
        zoxp_destroy,
        [in] rendering.RenderDistanceDirty,
        [in] rendering.RenderDistance,
        [in] tunks.TunkPosition,
        [in] tunks.Chunk3Stack,
        [none] streaming.StreamedChunk,
        [none] tunks.Tunk
    );
    // Streaming!
    if (!dbg_use_new_streaming) {
        zox_system(
            TunkRegionLinkSystem,
            zoxp_update,
            [in] tunks.TunkPosition,
            [out] regions.RegionLink,
            [none] tunks.Tunk
        );
        zox_system_1(
            FirstTerrainTunkSystem,
            zoxp_mainthread,
            [in] streaming.StreamLink,
            [in] streaming.StreamPosition2,
            [in] streaming.StreamDirty2,
            [none] streaming.Streamer
        );
        zox_filter(
            streamers,
            [in] streaming.StreamerLevel,
            [in] streaming.StreamLink,
            [in] streaming.StreamPosition2,
            [none] streaming.Streamer
        );
        zox_system_ctx_1(
            TunksSpawnSystem,
            zoxp_mainthread,
            streamers,
            [in] tunks.TunkPosition,
            [in] rendering.RenderDistance,
            [out] chunks2.Chunk2Neighbors,
            [none] streaming.StreamedChunk,
            [none] tunks.Tunk
        );
        zox_system(
            TunkLinkSystem,
            zoxp_update, // EcsPreUpdate,
            [in] core.Generate,
            [in] tunks.TunkPosition,
            [out] tunks.Chunk3Stack,
            [out] tunks.Chunk3Stacked,
            [none] tunks.Tunk
        );
    }
    // For now leave here
    zox_filter(
        streamers_lod,
        [in] streaming.StreamDirty2,
        [in] streaming.StreamerLevel,
        [in] streaming.StreamLink,
        [in] streaming.StreamPosition2,
        [none] streaming.Streamer
    );
    zox_system_ctx(
        TunkLodSystem,
        zoxp_update,
        streamers_lod,
        [in] tunks.TunkPosition,
        [in] tunks.Chunk3Stack,
        [out] rendering.RenderDistance,
        // [out] rendering.RenderDistanceDirty,
        [out] tunks.TunkLod,
        [out] tunks.GenerateTunk,
        [none] streaming.StreamedChunk
    );
    // Texture
    zox_system(
        TunkTextureSystem,
        zoxp_update,
        [in] tunks.TunkLink,
        [out] textures.GenerateTexture,
        [out] textures.TextureData,
        [out] rendering.TextureSize,
        [none] tunks.TunkTexture
    );
    /*zox_system(
        HeightmapTextureSystem,
        zoxp_update,
        [in] core.Generate,
        [in] tunks.TunkLink,
        [out] textures.TextureData,
        [out] rendering.TextureSize,
        [none] tunks.HeightsTexture
    );*/
}
