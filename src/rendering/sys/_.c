#include "initialize.c"
#include "lod_instance_system.c"
#include "settings.c"

void zox_systems_rendering(ecs *world) {
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
    // its settings: spawned on the app, applied when one changes (needs prefab_app)
    add_hook_spawn_prefabs(rendering_settings_events);
}
