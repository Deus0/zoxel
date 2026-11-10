#include "saving.c"
#include "render_depth.c"
#include "skeleton_meshes.c"
#include "debug_types.c"
#include "realm.c"

void zox_systems_characters3(ecs *world) {
    zox_system(
        CharacterRenderDepthSystem,
        zoxp_update,
        [in] rendering.RenderDepthDirty,
        [none] Character3,
        [none] !bones.Skeleton,
    );
    // Spawns, reuses and swaps a skeleton mesh per render depth
    // NOTE: Single threaded as it spawns entities, like ChunkMeshSpawnSystem
    zox_system_1(
        SkeletonMeshesSystem,
        zoxp_spawn,
        [in] rendering.RenderDepth,
        [in] chunks.NodeDepth,
        [in] colorz.ColorRGBs,
        [none] Character3,
        [none] bones.Skeleton,
    );
    // Builds skeleton meshes from their parent characters voxels
    zox_system(
        SkeletonMeshBuildSystem,
        zoxp_octree_read,
        [in] rendering.RenderDepth,
        [out] rendering.MeshIndicies,
        [out] rendering.MeshVertices,
        [out] rendering.MeshColorRGBs,
        [none] rendering3.SkeletonMesh,
        [none] rendering.BuildMesh,
    );
    zox_system(
        CharacterSaveSystem,
        zoxp_update,
        [in] transforms3.Position3D,
        [in] transforms3.Euler,
        [out] saves.SaveHash,
        [none] saves.Saver
    );
}

/*zox_system(
    Character3TypeDebugSystem,
    zoxp_update,
    [in] lines3.DebugCubeLines,
    [in] characters3.Character3Type,
    [out] colorz.Color,
    [none] Character3
);*/
