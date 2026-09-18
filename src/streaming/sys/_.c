#include "stream_point.c"
#include "frustum.c"
#include "stream_end.c"
#include "settings.c"

void zox_systems_streaming(ecs* world) {
    zox_system(
        StreamPositionSystem,
        zoxp_update,
        [in] streaming.StreamLink,
        [in] transforms3.Position3D,
        [out] streaming.StreamPosition,
        [out] streaming.StreamPosition2,
        [out] streaming.StreamDirty,
        [out] streaming.StreamDirty2,
        [none] streaming.Streamer
    );
    zox_filter(
        frustum_cameras,
        [in] transforms3.Position3DBounds,
        [in] cameras.CameraPlanes,
        [none] streaming.Streamer
    );
    zox_system_ctx(
        chunk_frustum_system,
        zoxp_update,
        frustum_cameras,
        [in] transforms3.Position3D,
        [in] transforms3.Bounds3D,
        [in] chunks3.VoxelNode,
        [in] chunks3.ChunkEntities,
        [out] rendering.RenderDisabled,
        [none] streaming.StreamedChunk
    );
    // add_system_process_counter(world, zox_id(chunk_frustum_system));
    // zox_set(zox_id(chunk_frustum_system), SystemDeltaMax, {  zox_lag_cutoff * 2 });
    // streams
    // main thread
    zox_system_1(
        StreamEndSystem,
        zoxp_spawn,
        [in] chunks3.ChunkLinks,
        [in] streaming.TerrainSpawnQueue,
        [in] StreamEndEvent,
        [none] saves.Loading,
    );
    // Settings
    zox_system_1(
        StreamingSettingsSystem,
        zoxp_spawn,
        [in] settings.LoadSettings
    );
    zox_system(
        StreamingSettingsDirtySystem,
        zoxp_update,
        [in] settings.SettingDirty,
        [in] core.ZoxName,
        [in] settings.Setting
    );
}
