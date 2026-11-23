// tst helpers: seams, every border between two chunks watertight as they're drawn (stream_seams, chunk_mesh_flicker)
//  - shares no code with the meshers (cpu sides, the gpu grid and shader), so a wrong rule there can't hide here:
//      - what a chunk draws: its shown meshes (ChunkMesh, not Disabled), each its material's voxels at its RenderDepth,
//        a voxel there found by walking its octree here (a closed node above that depth is its whole cube)
//      - its faces: read from the mesh itself, cpu its MeshVertices, gpu its vertex buffer read back (as many faces as
//        its draw command's index count): what the gpu draws, not what it was asked to
//  - a face is owed on a border cell where one side draws a voxel of a mesh's material and the other side draws
//    nothing there that hides it (block_material_hides: a solid, or the same material): owed and not drawn is a seam,
//    a hole through the wall; drawn and not owed is hidden behind the other side, counted apart (not seen)
//  - at the chunk's voxel resolution (its terrain's NodeDepth): a face covers the border cells it overlaps
//  - a chunk is compared once it's ready: generated, and each of its materials shows a built mesh
#define zox_tst_seam_meshes 8
#define zox_tst_seam_slots 16384    // a power of two, over twice the chunks streamed
#define zox_tst_seam_logs 30        // seams logged a run

typedef struct {
    entity chunk;
    byte filled;            // its drawing worked out (zox_tst_seam_fill)
    byte ready;             // generated, each material shows a built mesh
    byte why;               // not ready: 1 generating, 2 voxels and no materials yet, 3 a material shows no built mesh
    byte unknown;           // a cpu mesh built but not uploaded: its arrays aren't what's drawn
    byte candidate;         // chunk_mesh_flicker: changed this frame, its borders checked
    byte count;
    byte material[zox_tst_seam_meshes];
    byte depth[zox_tst_seam_meshes];
    entity mesh[zox_tst_seam_meshes];
    byte* cover[zox_tst_seam_meshes];   // 6 borders, side * side cells each: 1 where the mesh has a face on it
} zox_tst_seam_drawn;

typedef struct {
    int pairs;          // borders between two ready chunks compared
    int lod_pairs;      // ... drawn at different depths
    int missing;        // cells owed a face and not drawn: seams
    int missing_lod;    // ... where the two sides draw at different depths
    int extra;          // faces drawn where the other side hides them
    int unready;        // chunks beside another that weren't ready (not compared)
    int unready_why[4]; // ... by why (zox_tst_seam_drawn)
    int unknown;        // chunks mid upload (not compared)
    int seam_pairs;     // borders with a seam
} zox_tst_seam_result;

zox_tst_seam_drawn zox_tst_seam_cache[zox_tst_seam_slots];
// more about a seam's two chunks, for a test that knows more (chunk_mesh_flicker: when each last changed)
void (*zox_tst_seam_log_more)(ecs* world, entity x, entity y, char* text, int length) = NULL;
int zox_tst_seam_logged = 0;

static inline zox_tst_seam_drawn* zox_tst_seam_slot(entity chunk) {
    uint index = (uint) ((chunk * 0x9e3779b97f4a7c15ULL) >> 50) & (zox_tst_seam_slots - 1);
    while (zox_tst_seam_cache[index].chunk && zox_tst_seam_cache[index].chunk != chunk) {
        index = (index + 1) & (zox_tst_seam_slots - 1);
    }
    zox_tst_seam_cache[index].chunk = chunk;
    return &zox_tst_seam_cache[index];
}

// forgets every chunk's drawing (the next check reads them again)
static inline void zox_tst_seam_reset() {
    for (int i = 0; i < zox_tst_seam_slots; i++) {
        for (byte j = 0; j < zox_tst_seam_meshes; j++) {
            free(zox_tst_seam_cache[i].cover[j]);
        }
    }
    memset(zox_tst_seam_cache, 0, sizeof(zox_tst_seam_cache));
}

// the voxel at p (at resolution, the chunk's voxels a side 2^resolution) as a mesh at depth sees it
static inline byte zox_tst_seam_voxel(const VoxelNode* node, byte resolution, byte depth, int3 p) {
    for (byte d = 0; d < depth && has_children_VoxelNode(node); d++) {
        const byte shift = resolution - 1 - d;
        const byte child = ((p.x >> shift) & 1) | (((p.y >> shift) & 1) << 1) | (((p.z >> shift) & 1) << 2);
        node = &get_children_VoxelNode(node)[child];
    }
    return node->value;
}

// a face's 4 corners, in border cells: on a border (axis at 0 or side), its cells marked in cover; 1 if it's on one
static inline byte zox_tst_seam_cover(byte* cover, const float3* corners, int side) {
    const float c[4][3] = {
        { corners[0].x, corners[0].y, corners[0].z }, { corners[1].x, corners[1].y, corners[1].z },
        { corners[2].x, corners[2].y, corners[2].z }, { corners[3].x, corners[3].y, corners[3].z } };
    const float eps = 0.01f;
    for (byte axis = 0; axis < 3; axis++) {
        if (fabsf(c[1][axis] - c[0][axis]) > eps || fabsf(c[2][axis] - c[0][axis]) > eps || fabsf(c[3][axis] - c[0][axis]) > eps) {
            continue;
        }
        byte positive;
        if (fabsf(c[0][axis]) < eps) {
            positive = 0;
        } else if (fabsf(c[0][axis] - side) < eps) {
            positive = 1;
        } else {
            return 0;   // inside the chunk
        }
        const byte a = axis == 0 ? 1 : 0, b = axis == 2 ? 1 : 2;
        float min_a = c[0][a], max_a = c[0][a], min_b = c[0][b], max_b = c[0][b];
        for (byte k = 1; k < 4; k++) {
            min_a = fminf(min_a, c[k][a]); max_a = fmaxf(max_a, c[k][a]);
            min_b = fminf(min_b, c[k][b]); max_b = fmaxf(max_b, c[k][b]);
        }
        byte* plane = cover + (axis * 2 + positive) * side * side;
        const int u0 = (int) floorf(min_a + eps), u1 = (int) ceilf(max_a - eps);
        const int v0 = (int) floorf(min_b + eps), v1 = (int) ceilf(max_b - eps);
        for (int u = u0 < 0 ? 0 : u0; u < u1 && u < side; u++) {
            for (int v = v0 < 0 ? 0 : v0; v < v1 && v < side; v++) {
                plane[u * side + v] = 1;
            }
        }
        return 1;
    }
    return 0;
}

// a mesh's faces on its chunk's borders, read from what it draws; its faces drawn (-1 if it can't be read), inside
//  the faces not on a border
//  - block_scale: a voxel's size at resolution (cpu vertices are in the chunk's units, gpu ones in voxels at its depth)
static inline int zox_tst_seam_mesh_faces(ecs* world, entity mesh, byte resolution, float block_scale, byte* cover, int* inside) {
    int on_border = 0;
    const int side = 1 << resolution;
    const byte depth = zox_getv(mesh, RenderDepth);
    if (zox_has(mesh, GpuMesh)) {
        // the renderer skips it: no faces counted yet, or no vertex array
        if (!zox_getv(mesh, MeshRenderCount) || !zox_getv(mesh, VaoGPULink)) {
            if (inside) {
                *inside = 0;
            }
            return 0;
        }
        uint indices = 0;
        if (!zox_gpu_storage_read(zox_getv(mesh, MeshCommandGPULink).x, 0, sizeof(uint), &indices)) {
            return -1;
        }
        uint faces = indices / 6;
        const uint room = zox_getv(mesh, MeshFaceRoom);
        if (faces > room) {
            faces = room;
        }
        if (!faces) {
            if (inside) {
                *inside = 0;
            }
            return 0;
        }
        float4* vertices = malloc((size_t) faces * 4 * sizeof(float4));
        if (!vertices || !zox_gpu_storage_read(zox_getv(mesh, MeshGPULink).y, 0, (size_t) faces * 4 * sizeof(float4), vertices)) {
            free(vertices);
            return -1;
        }
        const float to_cells = (float) (1 << (resolution - depth));
        for (uint f = 0; f < faces; f++) {
            float3 corners[4];
            for (byte k = 0; k < 4; k++) {
                const float4 v = vertices[f * 4 + k];
                corners[k] = (float3) { v.x * to_cells, v.y * to_cells, v.z * to_cells };
            }
            on_border += zox_tst_seam_cover(cover, corners, side);
        }
        free(vertices);
        if (inside) {
            *inside = (int) faces - on_border;
        }
        return (int) faces;
    }
    const MeshVertices* vertices = zox_get(mesh, MeshVertices);
    const int faces = vertices->value ? vertices->length / 4 : 0;
    for (int f = 0; f < faces; f++) {
        float3 corners[4];
        for (byte k = 0; k < 4; k++) {
            const float3 v = vertices->value[f * 4 + k];
            corners[k] = (float3) { v.x / block_scale, v.y / block_scale, v.z / block_scale };
        }
        on_border += zox_tst_seam_cover(cover, corners, side);
    }
    if (inside) {
        *inside = faces - on_border;
    }
    return faces;
}

// a mesh shown and built once (what it draws can be read, faces or none)
static inline byte zox_tst_seam_mesh_drawn(ecs* world, entity mesh) {
    return zox_has(mesh, ChunkMesh) && !zox_has(mesh, Disabled) && zox_has(mesh, MeshBuilt);
}

// what a chunk draws, worked out once a check
static inline zox_tst_seam_drawn* zox_tst_seam_fill(ecs* world, entity chunk, byte resolution, float block_scale) {
    zox_tst_seam_drawn* drawn = zox_tst_seam_slot(chunk);
    if (drawn->filled) {
        return drawn;
    }
    drawn->filled = 1;
    if (!zox_valid(chunk) || !zox_has(chunk, VoxelNode) || (zox_has(chunk, GenerateChunk) && zox_getv(chunk, GenerateChunk))) {
        drawn->why = 1;
        return drawn;
    }
    const int side = 1 << resolution;
    entity materials[zox_block_materials];
    const uint materials_count = get_chunk_materials(world, chunk, materials);
    const VoxelNode* voxels = zox_get(chunk, VoxelNode);
    // voxels and no materials yet: its meshes aren't spawned
    if (!materials_count && (has_children_VoxelNode(voxels) || voxels->value)) {
        drawn->why = 2;
        return drawn;
    }
    byte ready = 1;
    for (uint i = 0; i < materials_count; i++) {
        entity meshes[zox_chunk_meshes_max];
        const uint count = zox_get_children_by_id(world, materials[i], meshes, zox_chunk_meshes_max, zox_id(ChunkMesh));
        byte shown = 0;
        for (uint j = 0; j < count; j++) {
            const entity mesh = meshes[j];
            if (!zox_tst_seam_mesh_drawn(world, mesh)) {
                continue;
            }
            if (!zox_has(mesh, GpuMesh) && zox_has(mesh, MeshDirty)) {
                drawn->unknown = 1;
            }
            if (drawn->count >= zox_tst_seam_meshes) {
                continue;
            }
            byte* cover = calloc(6 * side * side, 1);
            if (zox_tst_seam_mesh_faces(world, mesh, resolution, block_scale, cover, NULL) < 0) {
                free(cover);
                drawn->unknown = 1;
                continue;
            }
            const byte k = drawn->count++;
            drawn->mesh[k] = mesh;
            drawn->material[k] = zox_getv(materials[i], BlockMaterial);
            drawn->depth[k] = zox_getv(mesh, RenderDepth);
            drawn->cover[k] = cover;
            shown = 1;
        }
        ready &= shown;
    }
    drawn->ready = ready;
    drawn->why = ready ? 0 : 3;
    return drawn;
}

// x's faces into y across x's border (axis, positive: its far side), against what y draws there
static inline void zox_tst_seam_side(ecs* world, entity cx, entity cy, const zox_tst_seam_drawn* x, const zox_tst_seam_drawn* y,
    byte axis, byte positive, const byte* materials, byte resolution, zox_tst_seam_result* result, int* pair_missing)
{
    const int side = 1 << resolution;
    const VoxelNode* vx = zox_get(cx, VoxelNode);
    const VoxelNode* vy = zox_get(cy, VoxelNode);
    const byte a = axis == 0 ? 1 : 0, b = axis == 2 ? 1 : 2;
    const byte direction = axis * 2 + positive;
    for (int u = 0; u < side; u++) {
        for (int v = 0; v < side; v++) {
            int px[3], py[3];
            px[axis] = positive ? side - 1 : 0;
            py[axis] = positive ? 0 : side - 1;
            px[a] = py[a] = u;
            px[b] = py[b] = v;
            const int3 pxi = { px[0], px[1], px[2] }, pyi = { py[0], py[1], py[2] };
            for (byte i = 0; i < x->count; i++) {
                const byte voxel = zox_tst_seam_voxel(vx, resolution, x->depth[i], pxi);
                if (!voxel || materials[voxel] != x->material[i]) {
                    continue;
                }
                byte hidden = 0;
                for (byte j = 0; j < y->count && !hidden; j++) {
                    const byte other = zox_tst_seam_voxel(vy, resolution, y->depth[j], pyi);
                    hidden = other && materials[other] == y->material[j] && block_material_hides(materials[other], x->material[i]);
                }
                const byte covered = x->cover[i][direction * side * side + u * side + v];
                if (!hidden && !covered) {
                    result->missing++;
                    (*pair_missing)++;
                    byte lod = 0;
                    for (byte j = 0; j < y->count; j++) {
                        lod |= y->depth[j] != x->depth[i];
                    }
                    result->missing_lod += lod;
                    if (zox_tst_seam_logged < zox_tst_seam_logs && *pair_missing == 1) {
                        zox_tst_seam_logged++;
                        const int3 p = zox_getv(cx, ChunkPosition);
                        char depths[64] = { 0 };
                        int n = 0;
                        for (byte j = 0; j < y->count && n < 56; j++) {
                            n += snprintf(depths + n, sizeof(depths) - n, " m%i:d%i:v%i", y->material[j], y->depth[j],
                                zox_tst_seam_voxel(vy, resolution, y->depth[j], pyi));
                        }
                        const entity m = x->mesh[i];
                        zox_log("[tst]  seam: chunk [%ix%ix%i] material [%i] depth [%i] voxel [%i] has no face %s%c at cell [%i %i] "
                            "(%s, mesh %s%s%s, swapping [%i] to [%i], its sides building [%i]), its neighbour (swapping [%i] to [%i]) draws:%s",
                            p.x, p.y, p.z, x->material[i], x->depth[i], voxel, positive ? "+" : "-", "xyz"[axis], u, v,
                            zox_has(m, GpuMesh) ? "gpu" : "cpu", zox_has(m, BuildDisabled) ? "X" : "", zox_has(m, BuildMesh) ? "B" : "",
                            zox_has(m, MeshDirty) ? "M" : "", zox_has(cx, ChunkLodDirty), zox_getv(cx, RenderDepth),
                            zox_has(cx, BuildChunkSides), zox_has(cy, ChunkLodDirty), zox_getv(cy, RenderDepth),
                            y->count ? depths : " nothing");
                        if (zox_tst_seam_log_more) {
                            char more[512] = { 0 };
                            zox_tst_seam_log_more(world, cx, cy, more, sizeof(more));
                            zox_log("[tst]    %s", more);
                        }
                    }
                } else if (hidden && covered) {
                    result->extra++;
                }
            }
        }
    }
}

// the border between a and its neighbour b (axis, positive: b is past a's far side), both ways
static inline void zox_tst_seam_pair(ecs* world, entity ca, entity cb, byte axis, byte positive, const byte* materials,
    byte resolution, float block_scale, zox_tst_seam_result* result)
{
    zox_tst_seam_drawn* a = zox_tst_seam_fill(world, ca, resolution, block_scale);
    zox_tst_seam_drawn* b = zox_tst_seam_fill(world, cb, resolution, block_scale);
    if (!a->ready || !b->ready) {
        result->unready++;
        result->unready_why[a->ready ? b->why : a->why]++;
        return;
    }
    if (a->unknown || b->unknown) {
        result->unknown++;
        return;
    }
    result->pairs++;
    byte lod = 0;
    for (byte i = 0; i < a->count; i++) {
        for (byte j = 0; j < b->count; j++) {
            lod |= a->depth[i] != b->depth[j];
        }
    }
    result->lod_pairs += lod;
    int pair_missing = 0;
    zox_tst_seam_side(world, ca, cb, a, b, axis, positive, materials, resolution, result, &pair_missing);
    zox_tst_seam_side(world, cb, ca, b, a, axis, !positive, materials, resolution, result, &pair_missing);
    result->seam_pairs += pair_missing > 0;
}

// a terrain's voxel resolution (its chunks' NodeDepth at most) and a voxel's size there
static inline byte zox_tst_seam_resolution(ecs* world, entity terrain, float* block_scale) {
    *block_scale = zox_getv(terrain, BlockScale);
    return zox_getv(terrain, NodeDepth);
}

// every border between a terrain's chunks (only the candidates' with only_candidates, chunk_mesh_flicker)
static inline void zox_tst_seam_check(ecs* world, entity terrain, byte only_candidates, zox_tst_seam_result* result) {
    float block_scale = 1;
    const byte resolution = zox_tst_seam_resolution(world, terrain, &block_scale);
    const ChunkLinks* links = zox_get(terrain, ChunkLinks);
    byte materials[256];
    entity manager = 0;
    iter it = zox_children(world, terrain);
    while (zox_children_next(it)) {
        for (int j = 0; j < it.count; j++) {
            const entity chunk = it.entities[j];
            if (!zox_has(chunk, Chunk3) || !zox_has(chunk, ChunkPosition)) {
                continue;
            }
            if (only_candidates && !zox_tst_seam_slot(chunk)->candidate) {
                continue;
            }
            const int3 position = zox_getv(chunk, ChunkPosition);
            for (byte d = 0; d < 6; d++) {
                const byte axis = d / 2, positive = d % 2;
                const int3 offset = { axis == 0 ? (positive ? 1 : -1) : 0, axis == 1 ? (positive ? 1 : -1) : 0,
                    axis == 2 ? (positive ? 1 : -1) : 0 };
                const entity neighbor = int3_hashmap_get(links->value, int3_add(position, offset));
                if (!zox_valid(neighbor) || !zox_has(neighbor, Chunk3)) {
                    continue;
                }
                // each border once: from its lower chunk (a candidate's border with a chunk that isn't, from it)
                if (!positive && (!only_candidates || zox_tst_seam_slot(neighbor)->candidate)) {
                    continue;
                }
                const entity chunk_manager = zox_get_link(world, chunk, BlockManagerLink);
                if (chunk_manager != manager) {
                    manager = chunk_manager;
                    get_block_materials(world, manager, materials);
                }
                zox_tst_seam_pair(world, chunk, neighbor, axis, positive, materials, resolution, block_scale, result);
            }
        }
    }
}
