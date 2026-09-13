#include "render3.c"
#include "render3_instance.c"
#include "mesh_update.c"
#include "mesh_update_characters3.c"
#include "render_chunks.c"
#include "textured_upload.c"
#include "textured_render.c"
#include "textured_colors_upload.c"
#include "vox_render.c"
#ifndef zox_disable_rendering_instances
    #include "vox_instance_render_system.c"
#endif
#include "weights_initialize.c"
#include "weights_upload.c"
#include "weights_render.c"
#include "element.c"

void zox_define_systems_basics3(ecs* world) {
    zox_render3_system(
        0,
        Chunk3TexturedRenderSystem,
        [in] rendering.RenderDisabled,
        [in] transforms.TransformMatrix,
        // [in] rendering.MaterialLink,
        [in] rendering.MeshGPULink,
        [in] rendering.UvsGPULink,
        [in] rendering.ColorsGPULink,
        [in] rendering.MeshRenderCount,
        [none] rendering3.TexturedMesh3,
        [none] !core.Disabled,
        // [none] !core.Initialize,
    );
    add_system_process_counter(world, zox_id(Chunk3TexturedRenderSystem));
    // skybox, block damage overlay!
    zox_render3_system(
        2,
        Basic3RenderSystem,
        [in] transforms.TransformMatrix,
        [in] colorz.Color,
        [in] rendering.Alpha,
        [in] rendering.Brightness,
        [in] rendering.MeshGPULink,
        [in] rendering.MeshIndicies,
        [in] rendering.MaterialGPULink,
        [in] rendering3.MaterialBasic3D,
        [none] rendering3.MeshBasic3,
        [none] !core.Initialize,
    );
    // characters
    zox_render3_system(
        1,
        VoxRenderSystem,
        [in] rendering.MeshIndicies,
        [in] rendering.MeshGPULink,
        [in] rendering.ColorsGPULink,
        [in] transforms.TransformMatrix,
        [in] rendering.RenderDisabled,
        [none] rendering.VoxMesh,
        // [none] rendering.MeshColorRGBs,
        // [none] !rendering.UvsGPULink,
        [none] !rendering3.SkeletonMesh,
        [none] !core.Initialize,
    );
    // generating bone indexes here
    zox_render3_system(
        1,
        Skeleton3RenderSystem,
        [in] rendering.MeshIndicies,
        [in] rendering.MeshGPULink,
        [in] rendering.ColorsGPULink,
        [in] rendering3.BoneIndexGPULink,
        [in] transforms.TransformMatrix,
        [in] rendering.RenderDisabled,
        [in] rendering3.BoneLinks,
        [none] rendering3.SkeletonMesh,
        // [none] rendering.MeshColorRGBs,
        // [none] !rendering.UvsGPULink,
        [none] !core.Initialize,
    );
    // unique textured meshes - Items
    // NOTE: These are materials themselves
    zox_render3_system(
        1,
        TexturedRenderSystem,
        [in] transforms.TransformMatrix,
        [in] rendering.MeshGPULink,
        [in] rendering.UvsGPULink,
        [in] rendering.ColorsGPULink,
        [in] rendering.MeshIndicies,
        [in] rendering.RenderDisabled,
        [in] rendering.MaterialGPULink,
        [in] rendering.TextureGPULink,
        [in] rendering3.MaterialTextured3D,
        [none] rendering3.TexturedMesh3,
        [none] !core.Initialize,
    );
    zox_render3_system(
        2,
        Element3RenderSystem,
        [in] rendering.RenderDisabled,
        [in] layouts.Layer,
        [in] transforms.TransformMatrix,
        [in] rendering.MeshGPULink,
        [in] rendering.UvsGPULink,
        [in] rendering.ColorsGPULink,
        [in] rendering.TextureGPULink,
        [none] rendering.SingleMaterial,
        [none] !core.Initialize
    );
#ifndef zox_disable_rendering_instances
    zox_render3_system(
        1,
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
        zoxp_gpu_upload,
        [in] rendering.MeshIndicies,
        [in] rendering.MeshVertices,
        [in] rendering.MeshGPULink,
        [none] rendering.MeshDirty,
        [none] !rendering.MeshUVs,
        [none] !rendering.MeshColorRGBs,
        [none] !core.Initialize,
    );
    zox_system_1(
        MeshUpdateCharacters3DSystem,
        zoxp_gpu_upload,
        [in] rendering.MeshIndicies,
        [in] rendering.MeshVertices,
        [in] rendering.MeshColorRGBs,
        [out] rendering.MeshGPULink,
        [out] rendering.ColorsGPULink,
        [out] rendering.MeshRenderCount,
        [none] rendering.MeshDirty,
        [none] rendering.MeshColorRGBs,
        [none] !rendering.MeshUVs,
        [none] !core.PreInitialize,
        [none] !core.Initialize,
    );
    // Uploads Terrain Chunks, Items, Skybox
    zox_system_1(
        textured_mesh_update_system,
        zoxp_gpu_upload,
        [in] rendering.MeshGPULink,
        [in] rendering.UvsGPULink,
        [in] rendering.ColorsGPULink,
        [out] rendering.MeshIndicies,
        [out] rendering.MeshVertices,
        [out] rendering.MeshUVs,
        [in] rendering.MeshColorRGBs,
        [out] rendering.MeshRenderCount,
        [none] rendering.MeshDirty,
        [none] rendering3.TexturedMesh3,
        [none] !core.Initialize,
    );
    zox_system_1(
        MeshColorsGpuSystem,
        zoxp_gpu_upload,
        [in] rendering.ColorsGPULink,
        [in] rendering.MeshColorRGBs,
        [none] rendering.MeshColorsDirty,
        [none] !core.Initialize,
    );
    zox_system_1(
        BoneIndexUploadSystem,
        zoxp_gpu_upload,
        [in] rendering3.BoneIndexes,
        [out] rendering3.BoneIndexGPULink,
        [none] rendering.SkeletonMeshDirty,
        [none] !core.Initialize,
    );
    // Initialize
    zox_system_1(
        BonesInitializeSystem,
        zoxp_gpu_upload,
        [out] rendering3.BoneIndexGPULink,
        [none] core.Initialize,
    );
}
