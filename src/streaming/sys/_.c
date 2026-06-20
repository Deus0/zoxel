#include "stream_point.c"
#include "frustum.c"
#include "stream_end.c"
#include "settings.c"

void define_systems_streaming(ecs* world) {
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
        ChunkFrustumSystem,
        EcsOnUpdate,
        frustum_cameras,
        [in] transforms3.Position3D,
        [in] transforms3.Bounds3D,
        [in] chunks3.VoxelNode,
        [in] chunks3.ChunkEntities,
        [out] rendering.RenderDisabled,
        [none] streaming.StreamedChunk
    );
    zox_set(zox_id(ChunkFrustumSystem), SystemDeltaMax, {  zox_lag_cutoff * 2 });
    add_system_process_counter(world, zox_id(ChunkFrustumSystem));
    // streams
    // main thread
    zox_system_1(
        StreamEndSystem,
        zoxp_mainthread,
        [in] core.EventInput,
        [in] voxes.ChunkLinks,
        [out] saves.Loaded,
        [out] StreamEndEvent
    );
    // Settings
    zox_system_1(
        StreamingSettingsSystem,
        zoxp_mainthread,
        [in] core.InitializeEntity,
        [none] apps.App
    );
    zox_system_1(
        StreamingSettingsDirtySystem,
        zoxp_mainthread,
        [in] settings.SettingDirty,
        [in] core.ZoxName,
        [in] settings.Setting
    );
}
