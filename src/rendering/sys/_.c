#include "dispose.c"
#include "restore.c"
#include "initialize.c"
#include "lod_instance_system.c"
#include "settings.c"

void zox_systems_rendering(ecs *world) {
    zox_systems_rendering_dispose(world);
    zox_systems_rendering_restore(world);
    // other
    zox_system_1(
        initialize_mesh_system,
        zoxp_gpu_upload,
        [out] rendering.MeshGPULink,
        [none] core.Initialize,
    );
    zox_system_1(
        initialize_mesh_uvs_system,
        zoxp_gpu_upload,
        [out] rendering.UvsGPULink,
        [none] core.Initialize,
    );
    zox_system_1(
        initialize_mesh_colors_system,
        zoxp_gpu_upload,
        [out] rendering.ColorsGPULink,
        [none] core.Initialize,
    );
    zox_system_1(
        initialize_mesh_texture_system,
        zoxp_gpu_upload,
        [out] rendering.TextureGPULink,
        [none] core.Initialize,
    );
    zox_system(
        lod_instance_system,
        zoxp_update,
        [in] rendering.RenderDepthDirty,
        [in] rendering.RenderDepth,
        [none] rendering.RendererInstance
    );
    zox_system_1(
        rendering_settings_system,
        zoxp_spawn,
        [in] settings.LoadSettings,
    );
    zox_system(
        rendering_settings_dirty_system,
        zoxp_update,
        [in] settings.SettingDirty,
        [in] core.ZoxName,
        [in] settings.Setting
    );
}
