#include "basic_upload.c"
#include "textured_upload.c"
#include "render2_instance.c"
#include "render2.c"

void define_systems_basics2D(ecs *world) {
    zox_system_1(
        Mesh2DUpdateSystem,
        zoxp_mainthread,
        [out] rendering.MeshDirty,
        [in] rendering.MeshIndicies,
        [in] rendering.MeshVertices2D,
        [in] rendering.MeshGPULink,
        [none] !rendering.MeshUVs,
        [none] !rendering.MeshColorRGBs
    );
    zox_system_1(
        Mesh2DUvsUpdateSystem,
        zoxp_mainthread,
        [in] rendering.MeshDirty,
        [in] rendering.MeshGPULink,
        [in] rendering.UvsGPULink,
        [in] rendering.MeshIndicies,
        [in] rendering.MeshVertices2D,
        [in] rendering.MeshUVs,
    );
    // 2D or 3D pipeline?
    zox_render3_system(
        0,
        RenderMaterial2DSystem,
        [in] transforms2.Position2,
        [in] transforms2.Rotation2D,
        [in] transforms.Scale1,
        [in] rendering.Brightness,
        [in] rendering.MaterialGPULink,
        [in] rendering.TextureGPULink,
        [none] !rendering.MeshGPULink
    );
}
