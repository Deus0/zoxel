#include "sides.c"
#include "build.c"
#include "wait.c"
#include "spawn.c"
#include "trigger.c"
#include "transition.c"

zox_sys2(ChunkMeshTestSystem) {
    zox_log(
        "ChunkMeshTestSystem [%d] table=%p",
        it->count,
        (void*) it->table
    );
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, BuildMesh);
    }
} zox_sys_end(ChunkMeshTestSystem);


zox_sys2(ChunkMeshTest2System) {
    zox_sys_world();

    ecs_query_t *query = ecs_query(world, {
        .expr =
        "[none] chunks.ChunkMesh,"
        "[none] rendering.RenderDepth,"
        "[none] rendering.BuildMesh,"
        "[none] !core.BuildDisabled"
    });

    if (!query) {
        zox_loge("Failed to create ChunkMesh debug query");
        return;
    }

    ecs_query_count_t count = ecs_query_count(query);

    zox_log("ChunkMesh DEBUG:");
    zox_log(" - results: %d", count.results);
    zox_log(" - entities: %d", count.entities);
    zox_log(" - tables: %d", count.tables);

    ecs_iter_t it = ecs_query_iter(world, query);

    int table_index = 0;
    int total_entities = 0;

    while (ecs_query_next(&it)) {
        zox_log(
            " - table[%d]=%p entities=%d",
            table_index,
            (void *) it.table,
                it.count
        );

        total_entities += it.count;
        table_index++;
    }

    zox_log(
        "ChunkMesh DEBUG TOTAL: tables=%d entities=%d",
        table_index,
        total_entities
    );

    ecs_query_fini(query);
} zox_sys_end(ChunkMeshTest2System);


void define_systems_chunks3_textured(ecs *world) {
    zox_system(
        ChunkSidesTriggerSystem,
        zoxp_update,
        [none] chunks3.VoxelNodePostDirty,
        [none] chunks.ChunkTextured
    );
    zox_system(
        ChunkNeighborsSidesTriggerSystem,
        zoxp_update,
        [in] chunks3.ChunkNeighbors,
        [none] chunks3.VoxelNodePostDirty,
        [none] chunks.ChunkTextured
    );
    // Builds our Terrain Chunk Mesh
    // NOTE: Requires reading voxel data
    zox_system(
        ChunkSidesSystem,
        zoxp_voxels_sides,
        [in] chunks.NodeDepth,
        [in] chunks3.ChunkNeighbors,
        [in] chunks3.VoxelNode,
        [out] chunks3.SidesOctree,
        [none] chunks.BuildChunkSides,
        [none] chunks.ChunkTextured,
        [none] !chunks3.VoxelNodeDirty,
    );
    zox_system_1(
        ChunkMeshSpawn2System,
        zoxp_spawn,
        [in] transforms.TransformMatrix,
        [in] rendering.RenderDisabled,
        [in] rendering.RenderDepth,
        [in] chunks3.VoxelNode,
        [out] chunks3.ChunkLodDirty,
        [out] chunks.ChunkMeshTimer,
        [none] chunks.ChunkTextured
    );
    zox_system_1(
        ChunkMeshSpawnSystem,
        zoxp_spawn,
        [in] transforms.TransformMatrix,
        [in] rendering.RenderDisabled,
        [in] rendering.RenderDepth,
        [in] chunks3.VoxelNode,
        [out] chunks.ChunkMeshTimer,
        [none] chunks3.VoxelNodePostDirty,
        [none] chunks.ChunkTextured
    );
    // hmmmm
    zox_system(
        ChunkMeshTransitionSystem,
        zoxp_update,
        [in] chunks3.ChunkLodDirty,
        [in] rendering.RenderDepth,
        [out] chunks.ChunkMeshTimer,
        [out] rendering.ActiveMesh,
        [out] rendering.PreparingMesh,
        [none] chunks.ChunkTextured,
        [none] !chunks.GenerateChunk,
        [none] !chunks.BuildChunkSides,
    );

    zox_system(
        ChunkTexturedBuildSystem,
        zoxp_voxels_mesh,
        [in] rendering.RenderDepth,
        [out] rendering.MeshIndicies,
        [out] rendering.MeshVertices,
        [out] rendering.MeshUVs,
        [out] rendering.MeshColorRGBs,
        [none] chunks.ChunkMesh,
        [none] rendering.BuildMesh,
        [none] !core.BuildDisabled,
    );

    /*zox_system(
        ChunkMeshTestSystem,
        zoxp_voxels_mesh,
        [none] chunks.ChunkMesh,
        [none] rendering.RenderDepth,
        [none] rendering.BuildMesh,
        [none] !core.BuildDisabled,
    );
    zox_system(
        ChunkMeshTest2System,
        zoxp_voxels_mesh,
        0   // no filter
    );*/
}
