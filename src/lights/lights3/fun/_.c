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
#include "sample.c"

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
        (zox_has(e, LightOctreeDirty) && zox_getv(e, LightOctreeDirty)) ||
        zox_get(e, SunlightQueue)->count ||
        zox_get(e, LightQueue)->count ||
        zox_get(e, DarkQueue)->count;
}

// a chunk, or one beside it, lighting or about to (generating, or its lod's voxels regenerating, ChunkLodDirty before
//  its meshes spawn: its lights cleared dark till it's lit again): a mesh already lit and relit now reads lights half
//  made, darker for a few frames then back (dark flashes, tst chunk_mesh_flicker), it keeps its lights till they settle
static inline byte chunk_grid_lights_settling(ecs* world, const entity* chunks) {
    if (zox_no_lights) {
        return 0;
    }
    for (byte j = 0; j < 27; j++) {
        const entity c = chunks[j];
        if (!zox_valid(c) || !zox_has(c, LightOctree)) {
            continue;
        }
        if (zox_chunk_lighting_busy(world, c) || (zox_has(c, GenerateChunk) && zox_getv(c, GenerateChunk)) ||
            (zox_has(c, ChunkLodDirty) && zox_getv(c, ChunkLodDirty) == zox_chunk_lod_dirty_generating))
        {
            return 1;
        }
    }
    return 0;
}

byte zox_chunk_lights_settling(ecs* world, entity chunk) {
    if (!zox_valid(chunk) || zox_no_lights) {
        return 0;
    }
    entity chunks[27];
    fetch_nearby_chunks(world, chunk, zox_get(chunk, ChunkNeighbors)->value, chunks);
    return chunk_grid_lights_settling(world, chunks);
}
