#include "render3.c"
#include "render3_instance.c"
#include "render_characters3.c"
#include"mesh_colors.c"
#include "mesh_update.c"
#include "mesh_update_textured3.c"
#include "mesh_update_characters3.c"
#include "textured_render_system.c"
#ifndef zox_disable_rendering_instances
    #include "vox_instance_render_system.c"
#endif

void define_systems_basics3D(ecs* world) {
    // skybox
    zox_render3_system(1,
        Basic3RenderSystem,
        [in] transforms.TransformMatrix,
        [in] colorz.Color,
        [in] rendering.Alpha,
        [in] rendering.Brightness,
        [in] rendering.MeshGPULink,
        [in] rendering.MeshIndicies,
        [in] rendering.MaterialGPULink,
        [none] MeshBasic3D
    );
    // unique textured meshes - items
    zox_render3_system(1,
        TexturedRenderSystem,
        [in] transforms.TransformMatrix,
        [in] rendering.MeshGPULink,
        [in] rendering.UvsGPULink,
        [in] rendering.ColorsGPULink,
        [in] rendering.MeshIndicies,
        [in] rendering.RenderDisabled,
        [in] rendering.MaterialGPULink,
        [in] rendering.TextureGPULink,
        [in] MaterialTextured3D,
        [none] TexturedMesh3D
    );
    // characters
    zox_render3_system(1,
        Characters3RenderSystem,
        [in] rendering.MeshIndicies,
        [in] rendering.MeshGPULink,
        [in] rendering.ColorsGPULink,
        [in] transforms.TransformMatrix,
        [in] rendering.RenderDisabled,
        [none] rendering.VoxMesh,
        [none] rendering.MeshColorRGBs,
        [none] !rendering.UvsGPULink,
        [none] !rendering3.SkeletonMesh
    );
#ifndef zox_disable_rendering_instances
    zox_render3_system(1,
        VoxInstanceRenderSystem,
        [in] transforms.TransformMatrix,
        [in] rendering.InstanceLink,
        [in] rendering.RenderDisabled,
        [none] rendering.RendererInstance
    );
#endif
    // upload gpu
    zox_system_1(
        MeshUpdateSystem,
        zoxp_mainthread,
        [in] rendering.MeshDirty,
        [in] rendering.MeshIndicies,
        [in] rendering.MeshVertices,
        [in] rendering.MeshGPULink,
        [none] !rendering.MeshUVs,
        [none] !rendering.MeshColorRGBs
    );
    zox_system_1(
        TexturedMeshUploadSystem,
        zoxp_mainthread,
        [in] rendering.MeshIndicies,
        [in] rendering.MeshVertices,
        [in] rendering.MeshUVs,
        [in] rendering.MeshColorRGBs,
        [in] rendering.MeshGPULink,
        [in] rendering.UvsGPULink,
        [in] rendering.ColorsGPULink,
        [in] rendering.MeshDirty,
        [out] rendering.MesnRenderCount
    );
    zox_system_1(
        MeshUpdateCharacters3DSystem,
        zoxp_mainthread,
        [in] rendering.MeshIndicies,
        [in] rendering.MeshVertices,
        [in] rendering.MeshColorRGBs,
        [in] rendering.MeshDirty,
        [out] rendering.MeshGPULink,
        [out] rendering.ColorsGPULink,
        [out] rendering.MesnRenderCount,
        [none] rendering.MeshColorRGBs,
        [none] !rendering.MeshUVs
    );
    zox_system_1(
        MeshColorsGpuSystem,
        zoxp_mainthread,
        [in] rendering.MeshColorsDirty,
        [in] rendering.MeshColorRGBs,
        [in] rendering.ColorsGPULink,
        [in] rendering.MeshVertices,
        [none] rendering.MeshColorRGBs
    );
}
