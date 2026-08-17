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
        zoxp_mainthread,
        [in] core.Initialize,
        [out] rendering.MeshGPULink
    );
    zox_system_1(
        InitializeMeshUvsSystem,
        zoxp_mainthread,
        [in] core.Initialize,
        [out] rendering.UvsGPULink
    );
    zox_system_1(
        InitializeMeshColorsSystem,
        zoxp_mainthread,
        [in] core.Initialize,
        [out] rendering.ColorsGPULink
    );
    zox_system_1(
        InitializeMeshTextureSystem,
        zoxp_mainthread,
        [in] core.Initialize,
        [out] rendering.TextureGPULink
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
        zoxp_mainthread,
        [in] settings.LoadSettings,
    );
    zox_system_1(
        RenderingSettingsDirtySystem,
        zoxp_mainthread,
        [in] settings.SettingDirty,
        [in] core.ZoxName,
        [in] settings.Setting
    );
}
