#include "dispose_mesh_system.c"
#include "dispose_colors_system.c"
#include "dispose_uvs_system.c"
#include "dispose_texture_system.c"
#include "dispose_shader_system.c"
#include "dispose_material_system.c"
#include "restore_mesh_system.c"
#include "restore_colors_system.c"
#include "restore_uvs_system.c"
#include "restore_texture_system.c"
#include "restore_shader_system.c"
#include "restore_materials_system.c"
#include "restore_meshdirty_system.c"
// other
#include "lod_instance_system.c"
#include "settings.c"
zox_increment_system_with_reset(MeshDirty, mesh_state_end);

void define_systems_rendering(ecs *world) {
    zoxd_system_increment(MeshDirty);
    // dispose
    zox_gpu_dispose_system(MeshGPUDisposeSystem, [in] MeshGPULink);
    zox_gpu_dispose_system(MeshUvsGPUDisposeSystem, [in] rendering.UvsGPULink);
    zox_gpu_dispose_system(MeshColorsGPUDisposeSystem, [in] rendering.ColorsGPULink);
    zox_gpu_dispose_system(TextureGPUDisposeSystem, [in] TextureGPULink);
    zox_gpu_dispose_system(ShaderDisposeSystem, [in] ShaderGPULink);
    zox_gpu_dispose_system(MaterialDisposeSystem, [in] MaterialGPULink);
    // restore
    zox_gpu_restore_system(MeshGPURestoreSystem, [out] MeshGPULink);
    zox_gpu_restore_system(UvsGPULinkRestoreSystem, [out] rendering.UvsGPULink);
    zox_gpu_restore_system(ColorsGPULinkRestoreSystem, [out] rendering.ColorsGPULink);
    zox_gpu_restore_system(TextureRestoreSystem, [out] TextureGPULink);
    zox_gpu_restore_system(TextureDirtyRestoreSystem, [out] rendering.TextureDirty);
    zox_gpu_restore_system(ShaderRestoreSystem, [in] ShaderSourceIndex, [out] ShaderGPULink);
    zox_gpu_restore_system(MaterialRestoreSystem, [in] ShaderLink, [out] MaterialGPULink);
    zox_gpu_restore_system(MeshDirtyRestoreSystem, [out] rendering.MeshDirty);
    // other
    zox_system(
        LodInstanceSystem,
        zoxp_update, // EcsPostUpdate,
        [in] rendering.RenderDepthDirty,
        [in] rendering.RenderDepth,
        [in] rendering.ModelLink,
        [out] rendering.InstanceLink
    );
    zox_system_1(
        RenderingSettingsSystem,
        zoxp_mainthread,
        [in] core.InitializeEntity,
        [none] apps.App
    );
    zox_system_1(
        RenderingSettingsDirtySystem,
        zoxp_mainthread,
        [in] settings.SettingDirty,
        [in] core.ZoxName,
        [in] settings.Setting
    );
}
