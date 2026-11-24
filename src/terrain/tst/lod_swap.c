// tst: lod_swap, chunks never flicker while they swap lod meshes, flying like perf_fly
//  - each frame, every terrain chunk: the mesh it draws (a ChunkMesh, not Disabled, on the gpu with faces)
//  - a hole: a chunk drew last frame and draws nothing now, its mesh hidden or gone (a frame of sky where land was);
//      a mesh rebuilt with no faces isn't one (a solid chunk whose neighbour now covers it), logged apart
//  - dark: it now draws a different mesh never lit (no MeshColorsBuilt: its light colours never uploaded), a dark
//      flash; one relighting still draws its last colours, fine
//  - checks: no holes, no dark swaps; logs the swaps it saw (a run with none proves nothing)
//  - the lod swap (chunk_mesh_transition_system) had timers to hide these, doc/done/streaming.md (plan 6)
//  - run: bash bsh/tst.sh lod_swap --stress --far 8 (zox_tst_seed=666 for the same land)
#define zox_tst_ls_size 16384   // a power of two, over twice the chunks streamed (Far 8: ~2000)
typedef struct {
    entity chunk;
    entity mesh;    // the mesh it drew
} zox_tst_ls_slot;
zox_tst_ls_slot zox_tst_ls_maps[2][zox_tst_ls_size];
byte zox_tst_ls_now = 0;
int zox_tst_ls_holes = 0, zox_tst_ls_dark = 0, zox_tst_ls_swaps = 0, zox_tst_ls_frames = 0;
// why the mesh it drew stopped: deleted, disabled, its gpu mesh gone, no faces; and holes while it swaps lod
int zox_tst_ls_why[4], zox_tst_ls_why_swapping = 0;

static inline zox_tst_ls_slot* zox_tst_ls_find(zox_tst_ls_slot* map, entity chunk) {
    uint index = (uint) ((chunk * 0x9e3779b97f4a7c15ULL) >> 50) & (zox_tst_ls_size - 1);
    while (map[index].chunk && map[index].chunk != chunk) {
        index = (index + 1) & (zox_tst_ls_size - 1);
    }
    return &map[index];
}

// the mesh a chunk draws, 0 if none
static inline entity zox_tst_ls_drawn(ecs* world, entity chunk) {
    entity it_meshes[zox_chunk_meshes_max];
    const uint it_count = get_chunk_meshes(world, chunk, it_meshes);
    for (uint i = 0; i < it_count; i++) {
        const entity mesh = it_meshes[i];
        if (zox_has(mesh, ChunkMesh) && !zox_has(mesh, Disabled) && zox_has(mesh, MeshGPULink) &&
            zox_get(mesh, MeshGPULink)->value.x && zox_getv(mesh, MeshRenderCount)) {
            return mesh;
        }
    }
    return 0;
}

// it shows a mesh that's built and up to date, with faces or not
static inline byte zox_tst_ls_shows_built(ecs* world, entity chunk) {
    entity it_meshes[zox_chunk_meshes_max];
    const uint it_count = get_chunk_meshes(world, chunk, it_meshes);
    for (uint i = 0; i < it_count; i++) {
        const entity m = it_meshes[i];
        if (zox_has(m, ChunkMesh) && !zox_has(m, Disabled) && zox_has(m, MeshBuilt) && !zox_has(m, BuildMesh) && !zox_has(m, MeshDirty)) {
            return 1;
        }
    }
    return 0;
}

// the first holes, with the chunk's meshes: d depth, D disabled, X build disabled, B building, M uploading, + built,
//  L lighting, C colours uploading, c its indices
static inline void zox_tst_ls_log_hole(ecs* world, entity chunk, entity old, byte why) {
    if (zox_tst_ls_holes > 12) {
        return;
    }
    char text[512];
    const VoxelNode* voxels = zox_get(chunk, VoxelNode);
    int n = snprintf(text, sizeof(text), "hole [%i] why [%i] lod dirty [%i] depth [%i] node depth [%i] voxels [%s%i] meshes:",
        zox_tst_ls_holes, why, zox_has(chunk, ChunkLodDirty) ? zox_getv(chunk, ChunkLodDirty) : -1, zox_getv(chunk, RenderDepth),
        zox_getv(chunk, NodeDepth), has_children_VoxelNode(voxels) ? "split " : "solid ", voxels->value);
    entity it_meshes[zox_chunk_meshes_max];
    const uint it_count = get_chunk_meshes(world, chunk, it_meshes);
    for (uint j = 0; j < it_count && n < 480; j++) {
        const entity m = it_meshes[j];
        if (!zox_has(m, ChunkMesh)) {
            continue;
        }
        n += snprintf(text + n, sizeof(text) - n, " [%s d%i %s%s%s%s%s%s%s c%u]", m == old ? "old" : "", zox_getv(m, RenderDepth),
            zox_has(m, Disabled) ? "D" : "", zox_has(m, BuildDisabled) ? "X" : "", zox_has(m, BuildMesh) ? "B" : "",
            zox_has(m, MeshDirty) ? "M" : "", zox_has(m, MeshBuilt) ? "+" : "", zox_has(m, BuildMeshColors) ? "L" : "",
            zox_has(m, MeshColorsDirty) ? "C" : "", zox_getv(m, MeshRenderCount));
    }
    zox_log("[tst] %s", text);
}

static inline void zox_tst_ls_frame(ecs* world) {
    if (!zox_valid(local_terrain)) {
        return;
    }
    zox_tst_ls_slot* before = zox_tst_ls_maps[zox_tst_ls_now];
    zox_tst_ls_slot* now = zox_tst_ls_maps[!zox_tst_ls_now];
    memset(now, 0, sizeof(zox_tst_ls_maps[0]));
    iter it = zox_children(world, local_terrain);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            const entity chunk = it.entities[i];
            if (!zox_has(chunk, Chunk3)) {
                continue;
            }
            const entity mesh = zox_tst_ls_drawn(world, chunk);
            const zox_tst_ls_slot* last = zox_tst_ls_find(before, chunk);
            if (zox_tst_ls_frames && last->chunk && last->mesh && mesh != last->mesh) {
                if (!mesh) {
                    const entity old = last->mesh;
                    const byte why = !zox_valid(old) ? 0 : zox_has(old, Disabled) ? 1 : !zox_get(old, MeshGPULink)->value.x ? 2 : 3;
                    zox_tst_ls_why[why]++;
                    // showing a mesh, built and enabled, is right even with no faces (a solid chunk covered)
                    if (why != 3 && !zox_tst_ls_shows_built(world, chunk)) {
                        zox_tst_ls_holes++;
                        zox_tst_ls_why_swapping += zox_has(chunk, ChunkLodDirty);
                        zox_tst_ls_log_hole(world, chunk, old, why);
                    }
                } else {
                    zox_tst_ls_swaps++;
                    if (!zox_has(mesh, MeshColorsBuilt)) {
                        zox_tst_ls_dark++;
                        if (zox_tst_ls_dark <= 8) {
                            zox_log("[tst] dark [%i] chunk lod dirty [%i] lights busy [%i] old [%s] new: d%i %s%s%s%s%s%s c%u",
                                zox_tst_ls_dark, zox_has(chunk, ChunkLodDirty) ? zox_getv(chunk, ChunkLodDirty) : -1,
                                zox_chunk_lighting_busy(world, chunk), zox_valid(last->mesh) && zox_has(last->mesh, Disabled) ? "disabled" : "shown",
                                zox_getv(mesh, RenderDepth), zox_has(mesh, BuildDisabled) ? "X" : "", zox_has(mesh, BuildMesh) ? "B" : "",
                                zox_has(mesh, MeshDirty) ? "M" : "", zox_has(mesh, MeshBuilt) ? "+" : "", zox_has(mesh, BuildMeshColors) ? "L" : "",
                                zox_has(mesh, MeshColorsDirty) ? "C" : "", zox_getv(mesh, MeshRenderCount));
                        }
                    }
                }
            }
            zox_tst_ls_slot* slot = zox_tst_ls_find(now, chunk);
            slot->chunk = chunk;
            slot->mesh = mesh;
        }
    }
    zox_tst_ls_now = !zox_tst_ls_now;
    zox_tst_ls_frames++;
}

byte zox_tst_lod_swap_ready(ecs* world) {
    const byte done = zox_tst_perf_fly_ready(world);
    if (zox_tst_fly_start) {
        zox_tst_ls_frame(world);
    }
    return done;
}

void zox_tst_lod_swap_check(ecs* world) {
    zox_tst_perf_fly_check(world);
    zox_log("[tst] [metric] lod_swap frames [%i] swaps [%i] holes [%i] dark [%i]",
        zox_tst_ls_frames, zox_tst_ls_swaps, zox_tst_ls_holes, zox_tst_ls_dark);
    zox_log("[tst] [metric] lod_swap holes: its mesh deleted [%i] disabled [%i] off the gpu [%i] no faces [%i], mid lod swap [%i]",
        zox_tst_ls_why[0], zox_tst_ls_why[1], zox_tst_ls_why[2], zox_tst_ls_why[3], zox_tst_ls_why_swapping);
    zox_tst_check(zox_tst_ls_swaps > 0, "no lod swaps seen, nothing tested");
    zox_tst_check(!zox_tst_ls_holes, "[%i] times a chunk drew nothing the frame after it drew (a hole)", zox_tst_ls_holes);
    zox_tst_check(!zox_tst_ls_dark, "[%i] swaps drew a mesh with its light colours unbuilt (dark)", zox_tst_ls_dark);
}

// A procedural chunk that already has finer voxels only changes mesh. Regenerating it can overwrite edits.
static entity zox_tst_coarsened_chunk;

static byte zox_tst_lod_coarsen_ready(ecs* world) {
    if (!zox_tst_coarsened_chunk) {
        zox_tst_coarsened_chunk = zox_new();
        zox_add(zox_tst_coarsened_chunk, TerrainChunk);
        zox_setv(zox_tst_coarsened_chunk, NodeDepth, 4);
        zox_setv(zox_tst_coarsened_chunk, RenderDepth, 1);
        zox_setv(zox_tst_coarsened_chunk, ChunkLodDirty, zox_chunk_lod_dirty_start);
        return 0;
    }
    return zox_getv(zox_tst_coarsened_chunk, ChunkLodDirty) != zox_chunk_lod_dirty_start;
}

static void zox_tst_lod_coarsen_check(ecs* world) {
    zox_tst_check(!zox_has(zox_tst_coarsened_chunk, GenerateChunk), "coarsening restarted voxel generation");
    zox_tst_check(zox_getv(zox_tst_coarsened_chunk, NodeDepth) == 4, "coarsening discarded the generated depth");
}

// Hold a neighbour's side build across an update, then let it finish. A refinement that starts
// during that update replaces the voxels while the displayed neighbour still hides its border.
static entity zox_tst_refining_chunk, zox_tst_border_neighbor;
static int zox_tst_refine_frames;

static byte zox_tst_lod_refine_border_ready(ecs* world) {
    if (!zox_tst_refining_chunk) {
        zox_chunks3_gpu_meshing = 0;
        zox_tst_border_neighbor = zox_new();
        zox_add(zox_tst_border_neighbor, Chunk3);
        zox_add(zox_tst_border_neighbor, BuildChunkSides);
        zox_tst_refining_chunk = zox_new();
        zox_add(zox_tst_refining_chunk, TerrainChunk);
        zox_set(zox_tst_refining_chunk, ChunkNeighbors, { { zox_tst_border_neighbor } });
        zox_setv(zox_tst_refining_chunk, NodeDepth, 1);
        zox_setv(zox_tst_refining_chunk, RenderDepth, 2);
        zox_set(zox_tst_refining_chunk, LightOctree, { .value = darklight });
        zox_setv(zox_tst_refining_chunk, LightLock, 0);
        zox_setv(zox_tst_refining_chunk, ChunkLodDirty, zox_chunk_lod_dirty_start);
        return 0;
    }
    if (++zox_tst_refine_frames == 2) {
        zox_tst_check(zox_getv(zox_tst_refining_chunk, NodeDepth) == 1 && !zox_has(zox_tst_refining_chunk, GenerateChunk),
            "replaced the displayed terrain before its neighbour could open the border");
        zox_remove(zox_tst_border_neighbor, BuildChunkSides);
    }
    return zox_tst_refine_frames > 2 && zox_getv(zox_tst_refining_chunk, NodeDepth) == 2;
}

static void zox_tst_lod_refine_border_check(ecs* world) {
    zox_tst_check(zox_has(zox_tst_refining_chunk, GenerateChunk), "refinement failed to resume after the border opened");
}

void zox_tsts_lod_swap(ecs* world) {
    zox_tst_add("lod_refine_border", zox_tst_lod_refine_border_ready, zox_tst_lod_refine_border_check, 10);
    zox_tst_add("lod_coarsen", zox_tst_lod_coarsen_ready, zox_tst_lod_coarsen_check, 10);
    zox_tst_add("lod_swap", zox_tst_lod_swap_ready, zox_tst_lod_swap_check, 120);
}
