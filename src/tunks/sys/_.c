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
// More Maps
#include "texture.c"
#include "texture_heights.c"

void define_systems_tunks(ecs* world) {
    zox_system(
        TunkEndSystem,
        zoxp_update,
        [in] tunks.Chunk3Stack,
        [out] tunks.GenerateTunk,
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
        [out] tunks.TunkLod,
        [out] tunks.GenerateTunk,
        [none] streaming.StreamedChunk
    );
    // Texture
    zox_system(
        TunkTextureSystem,
        zoxp_update,
        [out] textures.GenerateTexture,
        [out] textures.TextureData,
        [out] rendering.TextureSize,
        [none] tunks.TunkTexture
    );
    /*zox_system(
        HeightmapTextureSystem,
        zoxp_update,
        [in] core.Generate,
        [out] textures.TextureData,
        [out] rendering.TextureSize,
        [none] tunks.HeightsTexture
    );*/
}
