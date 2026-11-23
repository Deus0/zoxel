#include "logs.c"
#include "prefab.c"
#include "fetch.c"
#include "nearby.c"
#include "solidity.c"
#include "emitters.c"
#include "openings.c"
#include "smooth_grid.c"
#include "gpu.c"
#include "relight.c"

byte zox_chunk_mesh_lighting_busy(ecs* world, entity e) {
    if (zox_no_lights) {
        return 0;
    }
    return zox_has(e, BuildMeshColors); // ||
        // !zox_has(e, MeshColorsBuilt);
}

byte zox_chunk_lighting_busy(ecs* world, entity e) {
    if (zox_no_lights) {
        return 0;
    }
    return
        (zox_has(e, LightNodeDirty) && zox_getv(e, LightNodeDirty)) ||
        zox_get(e, SunlightQueue)->count ||
        zox_get(e, LightQueue)->count ||
        zox_get(e, DarkQueue)->count;
}

// a chunk, or one beside it, lighting or about to (generating, or its lod's voxels regenerating, ChunkLodDirty before
//  its meshes spawn: its lights cleared dark till it's lit again): a mesh already lit and relit now reads lights half
//  made, darker for a few frames then back (dark flashes, tst chunk_mesh_flicker), it keeps its lights till they settle
byte zox_chunk_lights_settling(ecs* world, entity chunk) {
    if (zox_no_lights || !zox_valid(chunk)) {
        return 0;
    }
    entity chunks[1 + chunk_neighbors_length];
    chunks[0] = chunk;
    const ChunkNeighbors* neighbors = zox_get(chunk, ChunkNeighbors);
    for (byte j = 0; j < chunk_neighbors_length; j++) {
        chunks[1 + j] = neighbors->value[j];
    }
    for (byte j = 0; j < 1 + chunk_neighbors_length; j++) {
        const entity c = chunks[j];
        if (!zox_valid(c) || !zox_has(c, LightNode)) {
            continue;
        }
        if (zox_chunk_lighting_busy(world, c) || (zox_has(c, GenerateChunk) && zox_getv(c, GenerateChunk)) ||
            (zox_has(c, ChunkLodDirty) && zox_getv(c, ChunkLodDirty) < zox_chunk_lod_dirty_spawn))
        {
            return 1;
        }
    }
    return 0;
}
