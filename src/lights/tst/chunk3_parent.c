// A fixture: a realm, then a chunk3 parent of its own (chunks3/pre/chunk3_parent.c) with 3 x 3 x 3 all air chunks round
//  the origin, lit (lights3/sys/generated.c): no terrain, streaming or player
//  - for tests of what happens in and between chunks: lights, meshes, edits (water_light)
//  - register the test with zox_tst_set_setup(name, NULL, 1): it runs on the menus, no game started
//  - its realm zox_tst_chunks_realm, a chunk by position zox_tst_chunk_at; the top layer (y 1) is under the sky
//  - edit a chunk with its VoxelNodeQueue, as a player's hits do
//  - its seed: zox_tst_seed, else 666
entity zox_tst_chunks_realm = 0;
entity zox_tst_chunks_parent = 0;
int zox_tst_chunks_frames = 0;

// chunks still busy: generating, building sides, swapping lods, lights flooding, or a mesh not yet built and lit
//  (cpu: BuildMesh, its lights BuildMeshColors, upload MeshDirty; gpu: those, and its count not back)
int zox_tst_perf_generating = 0;
int zox_tst_perf_siding = 0;
int zox_tst_perf_meshing = 0;
int zox_tst_perf_swapping = 0;     // chunks changing lod (ChunkLodDirty)
int zox_tst_perf_lighting = 0;     // chunks whose lights are flooding
int zox_tst_perf_editing = 0;      // chunks with edits queued or applying (VoxelNodeQueue, VoxelNodeDirty, ChunkUpdate)

// a chunk3 parent's chunks still busy, counted by why (zox_tst_perf_*)
static inline int zox_tst_chunks_busy(ecs* world, entity parent) {
    zox_tst_perf_generating = 0;
    zox_tst_perf_swapping = 0;
    zox_tst_perf_siding = 0;
    zox_tst_perf_meshing = 0;
    zox_tst_perf_lighting = 0;
    zox_tst_perf_editing = 0;
    if (!zox_valid(parent)) {
        return 0;
    }
    iter it2 = zox_children(world, parent);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            const entity chunk = it2.entities[j];
            if (!zox_has(chunk, Chunk3)) {
                continue;
            }
            zox_tst_perf_generating += zox_has(chunk, GenerateChunk) && zox_getv(chunk, GenerateChunk);
            zox_tst_perf_siding += zox_has(chunk, BuildChunkSides);
            zox_tst_perf_swapping += zox_has(chunk, ChunkLodDirty);
            zox_tst_perf_lighting += zox_chunk_lighting_busy(world, chunk);
            zox_tst_perf_editing += (zox_has(chunk, VoxelNodeQueue) && zox_get(chunk, VoxelNodeQueue)->count) ||
                zox_has(chunk, VoxelNodeDirty) || zox_has(chunk, ChunkUpdate);
            entity it3_meshes[zox_chunk_meshes_max];
            const uint it3_count = get_chunk_meshes(world, chunk, it3_meshes);
            for (uint k = 0; k < it3_count; k++) {
                const entity mesh = it3_meshes[k];
                // old meshes (BuildDisabled, a swapped out lod) never build, they don't hold up streaming
                if (!zox_has(mesh, ChunkMesh) || zox_has(mesh, BuildDisabled)) {
                    continue;
                }
                zox_tst_perf_meshing += zox_has(mesh, BuildMesh) || zox_has(mesh, MeshDirty) ||
                    zox_has(mesh, BuildMeshColors) || zox_has(mesh, MeshColorsDirty) ||
                    (zox_has(mesh, GpuMesh) && zox_getv(mesh, MeshFence));
            }
        }
    }
    return zox_tst_perf_generating + zox_tst_perf_siding + zox_tst_perf_meshing + zox_tst_perf_swapping +
        zox_tst_perf_lighting + zox_tst_perf_editing;
}

entity zox_tst_chunk_at(ecs* world, int3 position) {
    return int3_hashmap_get(zox_get(zox_tst_chunks_parent, ChunkLinks)->value, position);
}

// a realm cube block by what it is, its BlockIndex, 0 if none: the first of a material (zox_block_material_*, solid
//  if it has none) giving
//  off light (BlockLight, 0 none); a solid one stopping light (leaves pass it), a liquid (water, oil) a source (BlockWaterLevel 8). Blocks are named by tags in terrain, after
//  this module (BlockWater, BlockBricks)
static inline byte zox_tst_chunks_block(ecs* world, byte material, byte light) {
    const BlockLinks* blocks = zox_get(zox_tst_chunks_realm, BlockLinks);
    for (int i = 0; i < blocks->length; i++) {
        const entity block = blocks->value[i];
        if (!zox_valid(block) || zox_has(block, BlockVox) || zox_has(block, BlockSize) ||
            (zox_has(block, BlockMaterial) ? zox_getv(block, BlockMaterial) : zox_block_material_solid) != material ||
            (zox_has(block, BlockLight) ? zox_getv(block, BlockLight) : 0) != light ||
            (material == zox_block_material_solid && zox_has(block, BlockLightPass) && zox_getv(block, BlockLightPass)) ||
            (block_material_liquid(material) && zox_has(block, BlockWaterLevel) && zox_getv(block, BlockWaterLevel) != 8))
        {
            continue;
        }
        return zox_getv(block, BlockIndex);
    }
    return 0;
}

// the parent once its realm has generated and its chunks are lit, 0 until then and while they're busy again (an edit
//  meshing and lighting): a test edits, then waits on this for the result
entity zox_tst_chunk3_parent_ready(ecs* world) {
    zox_tst_chunks_frames++;
    if (!zox_tst_chunks_realm) {
        zox_tst_waiting = "a realm of its own";
        if (!zox_valid(local_game) || zox_tst_chunks_frames < 5) {
            return 0;
        }
        const char* seed_text = getenv("zox_tst_seed");
        const lint seed = seed_text && seed_text[0] ? atol(seed_text) : 666;
        zox_tst_chunks_realm = spawn_realm(world, local_game, seed);
        zox_setv(zox_tst_chunks_realm, GenerateRealm, zox_generate_realm_start);
        return 0;
    }
    if (zox_getv(zox_tst_chunks_realm, GenerateRealm)) {
        return 0;
    }
    if (!zox_tst_chunks_parent) {
        zox_tst_chunks_parent = spawn_chunk3_parent_textured(world, prefab_chunk3_parent, zox_tst_chunks_realm,
            zox_tst_chunks_realm, zox_get_link(world, zox_tst_chunks_realm, Tilemap), terrain_depth, 0.5f);
        for (int x = -1; x <= 1; x++) for (int y = -1; y <= 1; y++) for (int z = -1; z <= 1; z++) {
            const entity chunk = spawn_chunk3_parent_chunk(world, prefab_chunk_lit, zox_tst_chunks_parent, (int3) { x, y, z },
                terrain_depth);
            // the sky over the top layer, beams end at the bottom (they'd wait for a chunk below)
            if (y == 1) {
                zox_add(chunk, SunnyChunk);
            } else if (y == -1) {
                zox_add(chunk, BottomChunk);
            }
            // its voxels are there (all air): lit, then meshed
            zox_setv(chunk, GenerateChunk, zox_generate_terrain_sunlight);
        }
        return 0;
    }
    if (!zox_tst_chunks_busy(world, zox_tst_chunks_parent)) {
        return zox_tst_chunks_parent;
    }
    static char waiting[160];
    snprintf(waiting, sizeof(waiting), "its chunks settled: generating [%i] sides [%i] meshing [%i] lods [%i] lighting [%i] "
        "editing [%i]", zox_tst_perf_generating, zox_tst_perf_siding, zox_tst_perf_meshing, zox_tst_perf_swapping,
        zox_tst_perf_lighting, zox_tst_perf_editing);
    zox_tst_waiting = waiting;
    return 0;
}
