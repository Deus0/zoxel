#include "render.c"

void define_systems_chunks3_textured(ecs *world) {

    if (!headless) {
        // move this into chunk3, for chunk3_textured
        zox_render3D_system(
            Chunk3TexturedRenderSystem,
            [in] transforms3.TransformMatrix,
            [in] rendering.MeshGPULink,
            [in] rendering.UvsGPULink,
            [in] rendering.ColorsGPULink,
            [in] rendering.MeshIndiciesGpu,
            [in] textures.TilemapLink,
            [in] rendering.RenderDisabled
        );
    }
}