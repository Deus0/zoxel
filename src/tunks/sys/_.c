// Link
#include "region_link.c"
// Streaming
#include "stream_start.c"
#include "stream_grow.c"
#include "stream_shrink.c"
// Stack
#include "stack_link.c"
#include "stack_trigger.c"
// More Maps
#include "texture.c"
#include "texture_heights.c"

void zox_systems_tunks(ecs* world) {
    zox_system(
        tunk_end_system,
        zoxp_update,
        [in] tunks.Chunk3Stack,
        [out] tunks.GenerateTunk,
        [none] tunks.Tunk
    );
    zox_system(
        tunk2_death_system,
        zoxp_destroy,
        [in] rendering.RenderDistanceDirty,
        [in] rendering.RenderDistance,
        [in] tunks.TunkPosition,
        [in] tunks.Chunk3Stack,
        [none] streaming.StreamedChunk,
        [none] tunks.Tunk
    );
    // Texture
    zox_system(
        tunk_texture_system,
        zoxp_update,
        [out] textures.TextureData,
        [out] rendering.TextureSize,
        [out] textures.GenerateTexture,
        [none] tunks.TunkTexture
    );
    /*zox_system(
        heightmap_texture_system,
        zoxp_update,
        [in] core.Generate,
        [out] textures.TextureData,
        [out] rendering.TextureSize,
        [none] tunks.HeightsTexture
    );*/
}
