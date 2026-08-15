#include "upload_basic.c"
#include "upload_textured.c"
#include "render_basic.c"
#include "render_instance.c"
// #include "render_layers.c"
#include "render_transform.c"

void define_systems_basics2D(ecs *world) {
    zox_system_1(
        Mesh2DUpdateSystem,
        zoxp_mainthread,
        [in] rendering.MeshIndicies,
        [in] rendering.MeshVertices2D,
        [in] rendering.MeshGPULink,
        [out] rendering.MeshDirty,
        [none] !rendering.MeshUVs,
        [none] !rendering.MeshColorRGBs,
        [none] !core.Initialize,
    );
    zox_system_1(
        MeshUVs2UploadSystem,
        zoxp_mainthread,
        [in] rendering.MeshGPULink,
        [in] rendering.UvsGPULink,
        [in] rendering.MeshIndicies,
        [in] rendering.MeshVertices2D,
        [in] rendering.MeshUVs,
        [out] rendering.MeshDirty,
        [none] !core.Initialize,
    );
    // 2D or 3D pipeline?
    /*zox_render2D_system(
        ElementRenderSystem,
        [in] rendering.RenderDisabled,
        [in] transforms2.Position2,
        [in] transforms2.Rotation2,
        [in] transforms.Scale1,
        [in] layouts.Layer,
        [in] rendering.Brightness,
        [in] rendering.Alpha,
        [in] rendering.MeshGPULink,
        [in] rendering.UvsGPULink,
        [in] rendering.TextureGPULink,
        [none] rendering2.TexturedMesh2,
        [none] !transforms.TransformMatrix,
        [none] !core.Initialize,
    );
    add_system_process_counter(world, zox_id(ElementRenderSystem));*/
    // all ui
    // Render using Matrix instead of Position2 etc
    zox_render2D_system(
        ElementRenderMatrixSystem,
        [in] rendering.RenderDisabled,
        [in] transforms.TransformMatrix,
        [in] layouts.Layer,
        [in] rendering.Brightness,
        [in] rendering.Alpha,
        [in] rendering.MeshGPULink,
        [in] rendering.UvsGPULink,
        [in] rendering.TextureGPULink,
        [none] rendering2.TexturedMesh2,
        [none] !core.Initialize,
    );
    zox_render3_system(0,
        RenderMaterial2DSystem,
        [in] transforms2.Position2,
        [in] transforms2.Rotation2,
        [in] transforms.Scale1,
        [in] rendering.Brightness,
        [in] rendering.MaterialGPULink,
        [in] rendering.TextureGPULink,
        [none] !rendering.MeshGPULink,
        [none] !core.Initialize,
    );
}
