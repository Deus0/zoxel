#include "dispose.c"
#include "restore.c"
#include "initialize.c"
#include "lod_instance_system.c"
#include "settings.c"

void define_systems_rendering(ecs *world) {
    zox_systems_rendering_dispose(world);
    zox_systems_rendering_restore(world);
    // other
    zox_system_1(
        InitializeMeshSystem,
        zoxp_gpu_upload,
        [out] rendering.MeshGPULink,
        [none] core.Initialize,
    );
    zox_system_1(
        InitializeMeshUvsSystem,
        zoxp_gpu_upload,
        [out] rendering.UvsGPULink,
        [none] core.Initialize,
    );
    zox_system_1(
        InitializeMeshColorsSystem,
        zoxp_gpu_upload,
        [out] rendering.ColorsGPULink,
        [none] core.Initialize,
    );
    zox_system_1(
        InitializeMeshTextureSystem,
        zoxp_gpu_upload,
        [out] rendering.TextureGPULink,
        [none] core.Initialize,
    );
    zox_system(
        LodInstanceSystem,
        zoxp_update,
        [in] rendering.RenderDepthDirty,
        [in] rendering.RenderDepth,
        [in] rendering.ModelLink,
        [out] rendering.InstanceLink
    );
    zox_system_1(
        RenderingSettingsSystem,
        zoxp_spawn,
        [in] settings.LoadSettings,
    );
    zox_system(
        RenderingSettingsDirtySystem,
        zoxp_update,
        [in] settings.SettingDirty,
        [in] core.ZoxName,
        [in] settings.Setting
    );
}
