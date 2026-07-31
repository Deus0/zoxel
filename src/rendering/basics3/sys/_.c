#include "render3.c"
#include "render3_instance.c"
#include "render_characters3.c"
#include "mesh_update.c"
#include "mesh_update_characters3.c"
#include "render_chunks.c"
#include "textured_upload.c"
#include "textured_render.c"
#include "textured_colors_upload.c"
#ifndef zox_disable_rendering_instances
    #include "vox_instance_render_system.c"
#endif
#include "element.c"

void zox_define_systems_basics3(ecs* world) {
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
        [in] rendering3.MaterialBasic3D,
        [none] rendering3.MeshBasic3D,
        [none] !core.Initialize,
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
        [none] !rendering3.SkeletonMesh,
        [none] !core.Initialize,
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
        [in] rendering.MeshIndicies,
        [in] rendering.MeshVertices,
        [in] rendering.MeshGPULink,
        [out] rendering.MeshDirty,
        [none] !rendering.MeshUVs,
        [none] !rendering.MeshColorRGBs,
        [none] !core.Initialize,
    );
    zox_system_1(
        MeshUpdateCharacters3DSystem,
        zoxp_mainthread,
        [in] rendering.MeshIndicies,
        [in] rendering.MeshVertices,
        [in] rendering.MeshColorRGBs,
        [out] rendering.MeshDirty,
        [out] rendering.MeshGPULink,
        [out] rendering.ColorsGPULink,
        [out] rendering.MeshRenderCount,
        [none] rendering.MeshColorRGBs,
        [none] !rendering.MeshUVs,
        [none] !core.Initialize,
    );
    // unique textured meshes - Items
    // NOTE: These are materials themselves
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
        [in] rendering3.MaterialTextured3D,
        [none] rendering3.TexturedMesh3D,
        [none] !core.Initialize,
    );
    // Uploads Terrain Chunks, Items, Skybox
    zox_system_1(
        TexturedMeshUploadSystem,
        zoxp_mainthread,
        [in] rendering.MeshGPULink,
        [in] rendering.UvsGPULink,
        [in] rendering.ColorsGPULink,
        [out] rendering.MeshIndicies,
        [out] rendering.MeshVertices,
        [out] rendering.MeshUVs,
        [in] rendering.MeshColorRGBs,
        [out] rendering.MeshDirty,
        [out] rendering.MeshRenderCount,
        [none] rendering3.TexturedMesh3D,
        [none] !core.Initialize,
    );
    zox_system_1(
        MeshColorsGpuSystem,
        zoxp_mainthread,
        [in] rendering.ColorsGPULink,
        [in] rendering.MeshColorRGBs,
        [out] rendering.MeshColorsDirty,
        [none] rendering.MeshColorRGBs,
        [none] !core.Initialize,
    );
    zox_render3_system(0,
        Chunk3TexturedRenderSystem,
        [in] rendering.RenderDisabled,
        [in] transforms.TransformMatrix,
        [in] rendering.MaterialLink,
        [in] rendering.MeshGPULink,
        [in] rendering.UvsGPULink,
        [in] rendering.ColorsGPULink,
        [in] rendering.MeshRenderCount,
        [none] rendering3.TexturedMesh3D, // chunks.ChunkMesh,
        [none] !core.Disabled,
        [none] !core.Initialize,
    );
    add_system_process_counter(world, zox_id(Chunk3TexturedRenderSystem));
    zox_render3_system(2,
        Element3RenderSystem,
        [in] rendering.RenderDisabled,
        [in] layouts.Layer2D,
        [in] transforms.TransformMatrix,
        [in] rendering.MeshGPULink,
        [in] rendering.UvsGPULink,
        [in] rendering.ColorsGPULink,
        [in] rendering.TextureGPULink,
        [none] rendering.SingleMaterial,
        [none] !core.Initialize
    );
}
