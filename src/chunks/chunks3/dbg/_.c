#include "label.c"
#include "memory.c"
#include "lines.c"
#include "chunk3.c"
#include "chunk3_mass.c"
#include "chunk3_textured.c"

void define_systems_chunks3_debug(ecs *world) {
    // main thread
    /*zox_system_1(
        ChunkDebugSystem,
        zoxp_voxels_read,
        [in] lines3.DebugCubeLines,
        [in] blocks.BlockScale,
        [in] transforms3.Position3D,
        [in] chunks3.VoxelNode,
        [in] chunks.NodeDepth,
        [in] rendering.RenderDistance,
        [in] chunks3.ChunkNeighbors,
        [none] ChunkDebugger
    );*/
}
