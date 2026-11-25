// tst: perf_meshing, how long the cpu meshers take a mesh, on the same voxels every run (doc/plans/chunks_refactor.md:
//  the meshers are merged and the textured one loses SidesOctree, a step mustn't make them slower)
//  - terrain: a chunk (terrain_depth, 16³) of hills, three blocks and caves, with its 6 neighbours, meshed at each lod
//      (16³ down to 1³ voxels) two ways:
//      - sides: SidesOctree built (build_sides_dig, what chunk_sides_system does), then the mesh from it, the game's now
//      - grid: the voxels as a byte grid with a border from the neighbours (sys/build_grid.c), faces by the gpu
//          mesher's rule; must make the same faces as sides, checked face for face (corners and uvs)
//  - models: a ball (32³) of 3 colours with holes, at each lod, both colored walkers: the pyramid grid
//      (build_voxel_mesh_grid) and the octree walk (build_voxel_mesh_c)
//  - each the fastest of 5 rounds of a few builds (the least disturbed by the rest of the machine), in ms a mesh;
//    repeat runs agree within 1%. Face counts are checked: a mesher change drawing other faces fails until updated
//  - a world: a stream's chunks at each lod (the default far 16's circle), each lod's time by its count: what meshing
//    a whole world once costs each way
//  - prints a table (the [tst] lines); baseline in doc/results.md (Meshing speed)
//  - run: bash bsh/tst.sh perf_meshing (debug), bash bsh/tst.sh perf_meshing --release (the speed that ships)
#define zox_tst_pm_rounds 5
#define zox_tst_pm_model_depth 5

// a hill height a column, the same every run
static inline int zox_tst_pm_height(int x, int z) {
    return 8 + (int) (4 * sinf(x * 0.37f) + 3 * cosf(z * 0.29f) + 2 * sinf((x + z) * 0.11f));
}

// a terrain chunk at chunk position c: stone, dirt, grass on top, air above, a few caves
static inline void zox_tst_pm_terrain(VoxelNode* node, byte depth, int3 c) {
    create_VoxelNode(node);
    const int length = octree_size(depth);
    for (int x = 0; x < length; x++) for (int y = 0; y < length; y++) for (int z = 0; z < length; z++) {
        const int wx = c.x * length + x, wy = c.y * length + y, wz = c.z * length + z;
        const int h = zox_tst_pm_height(wx, wz);
        byte value = wy > h ? 0 : (wy == h ? 3 : (wy > h - 3 ? 2 : 1));
        if (value && wy < h - 2 && (wx * 7 + wy * 13 + wz * 5) % 17 == 0) {
            value = 0;  // caves
        }
        set_VoxelNode(node, depth, (byte3) { x, y, z }, value);
    }
    optimize_reduce_VoxelNode(node);  // as the game's chunks are: solid areas big nodes, parents a value (sides need it)
}

// a model: a ball of 3 colours with holes, optimised as models are (parents get values, the lower lods)
static inline void zox_tst_pm_model(VoxelNode* node, byte depth) {
    create_VoxelNode(node);
    const int length = octree_size(depth);
    const float radius = length * 0.45f, center = (length - 1) * 0.5f;
    for (int x = 0; x < length; x++) for (int y = 0; y < length; y++) for (int z = 0; z < length; z++) {
        const float dx = x - center, dy = y - center, dz = z - center;
        byte value = dx * dx + dy * dy + dz * dz <= radius * radius ? 1 + (x + y * 3) % 3 : 0;
        if ((x * 7 + y * 13 + z * 5) % 11 == 0) {
            value = 0;
        }
        set_VoxelNode(node, depth, (byte3) { x, y, z }, value);
    }
    optimize_reduce_VoxelNode(node);
}

// a row of the results table
typedef struct {
    const char* what;
    int lod;            // voxels a side
    const char* how;
    int faces;
    double prepare_ms;  // sides built, or the grid filled; 0 for none
    double mesh_ms;
    const char* note;
} zox_tst_pm_row;

zox_tst_pm_row zox_tst_pm_rows[20];

// a streamed world at terrain_lod_far meshed once, sides against the grid
typedef struct {
    int chunks;
    double sides_ms;
    double grid_ms;
} zox_tst_pm_world_mix;
zox_tst_pm_world_mix zox_tst_pm_world;
char zox_tst_pm_world_lods[96];
int zox_tst_pm_row_count = 0;

// the faces each mesh made the first time this ran (2026-11-25): a mesher change that draws other faces fails here
typedef struct {
    const char* what;
    const char* how;
    int lod;
    int faces;
} zox_tst_pm_count;

static const zox_tst_pm_count zox_tst_pm_expected[] = {
    { "terrain", "sides", 16, 1206 }, { "terrain", "sides", 8, 137 }, { "terrain", "sides", 4, 31 },
    { "terrain", "grid", 16, 1206 }, { "terrain", "grid", 8, 137 }, { "terrain", "grid", 4, 31 },
    { "terrain", "sides", 2, 4 }, { "terrain", "sides", 1, 1 }, { "terrain", "grid", 2, 4 }, { "terrain", "grid", 1, 1 },
    { "terrain", "sides*", 8, 202 }, { "terrain", "sides*", 4, 81 },
    { "terrain", "grid*", 8, 202 }, { "terrain", "grid*", 4, 81 },
    { "model", "pyramid", 32, 10022 }, { "model", "pyramid", 16, 5566 }, { "model", "pyramid", 8, 1798 },
    { "model", "walk", 32, 10022 }, { "model", "walk", 16, 5566 }, { "model", "walk", 8, 1798 },
};

static inline void zox_tst_pm_add(const char* what, int lod, const char* how, int faces, double prepare_ms, double mesh_ms,
    const char* note)
{
    for (uint i = 0; i < sizeof(zox_tst_pm_expected) / sizeof(zox_tst_pm_expected[0]); i++) {
        const zox_tst_pm_count* e = &zox_tst_pm_expected[i];
        if (!strcmp(e->what, what) && !strcmp(e->how, how) && e->lod == lod) {
            zox_tst_check(faces == e->faces, "%s %i³ %s made [%i] faces, it made [%i] (the mesher's output changed)",
                what, lod, how, faces, e->faces);
        }
    }
    zox_tst_check(faces > 0, "%s %i³ %s made no faces", what, lod, how);
    if (zox_tst_pm_row_count < 20) {
        zox_tst_pm_rows[zox_tst_pm_row_count++] = (zox_tst_pm_row) { what, lod, how, faces, prepare_ms, mesh_ms, note };
    }
}

// a mesh's faces as 4 corners and 4 uvs, sorted, so two meshes compare without their order
typedef struct {
    float3 vertices[4];
    float2 uvs[4];
} zox_tst_pm_face;

static int zox_tst_pm_face_compare(const void* a, const void* b) {
    return memcmp(a, b, sizeof(zox_tst_pm_face));
}

static zox_tst_pm_face* zox_tst_pm_faces(const mesh_uvs_build_data* mesh, int* count) {
    *count = mesh->vertices->size / 4;
    zox_tst_pm_face* faces = calloc(*count ? *count : 1, sizeof(zox_tst_pm_face));
    for (int i = 0; i < *count; i++) {
        for (byte j = 0; j < 4; j++) {
            faces[i].vertices[j] = mesh->vertices->data[i * 4 + j];
            faces[i].uvs[j] = mesh->uvs->data[i * 4 + j];
        }
    }
    qsort(faces, *count, sizeof(zox_tst_pm_face), zox_tst_pm_face_compare);
    return faces;
}

static inline mesh_uvs_build_data zox_tst_pm_uvs_mesh() {
    return (mesh_uvs_build_data) {
        .indicies = create_int_array_d(initial_dynamic_array_size),
        .vertices = create_float3_array_d(initial_dynamic_array_size),
        .uvs = create_float2_array_d(initial_dynamic_array_size),
        .color_rgbs = create_color_rgb_array_d(initial_dynamic_array_size)
    };
}

static inline void zox_tst_pm_uvs_mesh_dispose(mesh_uvs_build_data* mesh) {
    dispose_int_array_d(mesh->indicies);
    dispose_float3_array_d(mesh->vertices);
    dispose_float2_array_d(mesh->uvs);
    dispose_color_rgb_array_d(mesh->color_rgbs);
}

// the terrain chunk's world for a mesher: tilemap uvs for 3 blocks (each face its own tile), all solid, full height
typedef struct {
    float2 tile_uvs[3 * 24];
    TilemapUVs tilemap_uvs;
    byte solidity[255];
    byte heights[255];
    byte drawn[256];    // the solid material's voxels
    byte hides[256];    // voxels hiding its faces
    VoxelNodeLock lock;
    byte* grid_buffer;  // the grid's, kept as a system keeps one for its run
} zox_tst_pm_terrain_world;

static inline void zox_tst_pm_terrain_world_init(zox_tst_pm_terrain_world* w) {
    for (int i = 0; i < 3 * 24; i++) {
        const int tile = i / 4;
        w->tile_uvs[i] = (float2) { (tile % 8) / 8.0f + ((i % 4) >= 2) / 8.0f, (tile / 8) / 8.0f + ((i % 4) == 1 || (i % 4) == 2) / 8.0f };
    }
    w->tilemap_uvs = (TilemapUVs) { .value = w->tile_uvs, .length = 3 * 24 };
    memset(w->solidity, 1, sizeof(w->solidity));
    memset(w->heights, 0, sizeof(w->heights));
    memset(w->drawn, 1, sizeof(w->drawn));
    memset(w->hides, 1, sizeof(w->hides));
    w->drawn[0] = w->hides[0] = 0;
    spinlock_init(&w->lock.value);
    w->grid_buffer = malloc(voxel_grid_cells(terrain_depth));
}

// one textured mesh, with sides (grid 0) or the grid (1): its prepare and mesh times added, its mesh kept if keep
static inline int zox_tst_pm_textured_once(zox_tst_pm_terrain_world* w, const VoxelNode* chunk, const VoxelNode** neighbors,
    const byte* neighbor_depths, byte target, byte grid, double* prepare_ms, double* mesh_ms, mesh_uvs_build_data* keep)
{
    mesh_uvs_build_data mesh = zox_tst_pm_uvs_mesh();
    const terrain_build_data data = {
        .tilemap_uvs = &w->tilemap_uvs,
        .voxel_solidity = w->solidity,
        .heights = w->heights,
        .mesh_data = &mesh,
    };
    double begin = get_time_ms();
    if (grid) {
        voxel_grid g;
        voxel_grid_fill(&g, w->grid_buffer, chunk, neighbors, neighbor_depths, target, w->hides);
        *prepare_ms += get_time_ms() - begin;
        begin = get_time_ms();
        build_textured_grid(&g, data, target, 8.0f, w->drawn, w->hides);
        *mesh_ms += get_time_ms() - begin;
    } else {
        SidesOctree sides;
        create_SidesOctree(&sides);
        build_sides_dig(NULL, zox_block_material_solid, chunk, neighbors, neighbor_depths, chunk, &sides, target, 0, byte3_zero, 0);
        *prepare_ms += get_time_ms() - begin;
        begin = get_time_ms();
        zox_terrain_building_dig(data, &w->lock, &sides, chunk, byte3_zero, 8.0f, target, 0, 0);
        *mesh_ms += get_time_ms() - begin;
        collapse_SidesOctree(&sides);
    }
    const int faces = mesh.vertices->size / 4;
    if (keep) {
        *keep = mesh;
    } else {
        zox_tst_pm_uvs_mesh_dispose(&mesh);
    }
    return faces;
}

// textured at one lod, one way: the fastest round's prepare and mesh ms a mesh, its faces
static int zox_tst_pm_textured(zox_tst_pm_terrain_world* w, const VoxelNode* chunk, const VoxelNode** neighbors,
    const byte* neighbor_depths, byte target, byte grid, double* prepare_ms, double* mesh_ms)
{
    const int builds = target >= 4 ? 10 : 40;
    int faces = 0;
    double best = -1;
    for (byte r = 0; r < zox_tst_pm_rounds; r++) {
        double prepare = 0, mesh = 0;
        for (int b = 0; b < builds; b++) {
            faces = zox_tst_pm_textured_once(w, chunk, neighbors, neighbor_depths, target, grid, &prepare, &mesh, NULL);
        }
        if (best < 0 || prepare + mesh < best) {
            best = prepare + mesh;
            *prepare_ms = prepare / builds;
            *mesh_ms = mesh / builds;
        }
    }
    return faces;
}

// the grid's faces against sides', face for face: how many differ (-1 a different count)
static int zox_tst_pm_textured_differ(zox_tst_pm_terrain_world* w, const VoxelNode* chunk, const VoxelNode** neighbors,
    const byte* neighbor_depths, byte target)
{
    double unused_a = 0, unused_b = 0;
    mesh_uvs_build_data sides_mesh, grid_mesh;
    zox_tst_pm_textured_once(w, chunk, neighbors, neighbor_depths, target, 0, &unused_a, &unused_b, &sides_mesh);
    zox_tst_pm_textured_once(w, chunk, neighbors, neighbor_depths, target, 1, &unused_a, &unused_b, &grid_mesh);
    int sides_count, grid_count;
    zox_tst_pm_face* sides_faces = zox_tst_pm_faces(&sides_mesh, &sides_count);
    zox_tst_pm_face* grid_faces = zox_tst_pm_faces(&grid_mesh, &grid_count);
    int differ = -1;
    if (sides_count == grid_count) {
        differ = 0;
        for (int i = 0; i < sides_count; i++) {
            differ += zox_tst_pm_face_compare(&sides_faces[i], &grid_faces[i]) != 0;
        }
    }
    free(sides_faces);
    free(grid_faces);
    zox_tst_pm_uvs_mesh_dispose(&sides_mesh);
    zox_tst_pm_uvs_mesh_dispose(&grid_mesh);
    return differ;
}

// colored at one lod, one walker: ms a mesh (the fastest round), its faces
static int zox_tst_pm_colored(const VoxelNode* model, const ColorRGBs* colors, byte depth, byte target, byte pyramid, double* ms) {
    const VoxelNode* noctrees[6] = { 0 };
    const byte nrdepths[6] = { 0 };
    const int builds = target >= 5 ? 5 : 20;
    int faces = 0;
    *ms = -1;
    for (byte r = 0; r < zox_tst_pm_rounds; r++) {
        double total = 0;
        for (int b = 0; b < builds; b++) {
            mesh_colored_build_data mesh = {
                .indicies = create_int_array_d(initial_dynamic_array_size),
                .vertices = create_float3_array_d(initial_dynamic_array_size),
                .colors = create_color_rgb_array_d(initial_dynamic_array_size)
            };
            const double begin = get_time_ms();
            if (pyramid) {
                build_voxel_mesh_grid(model, colors, &mesh, depth, target, float3_zero, 1.0f, 1);
            } else {
                build_voxel_mesh_c(model, model, noctrees, nrdepths, colors, &mesh, target, 0, byte3_zero, float3_zero, 1.0f, 1);
            }
            total += get_time_ms() - begin;
            faces = mesh.vertices->size / 4;
            dispose_int_array_d(mesh.indicies);
            dispose_float3_array_d(mesh.vertices);
            dispose_color_rgb_array_d(mesh.colors);
        }
        if (*ms < 0 || total / builds < *ms) {
            *ms = total / builds;
        }
    }
    return faces;
}

byte zox_tst_perf_meshing_ready(ecs* world) {
    (void) world;
    return 1;
}

static void zox_tst_pm_print() {
    zox_log("[tst] perf_meshing: ms to mesh once on the cpu, the fastest of %i rounds (lower is better)", zox_tst_pm_rounds);
    zox_log("[tst]   %-8s %-5s %-8s %6s %9s %9s %9s", "what", "lod", "how", "faces", "prepare", "write", "total");
    for (int i = 0; i < zox_tst_pm_row_count; i++) {
        const zox_tst_pm_row* r = &zox_tst_pm_rows[i];
        char lod[8];
        snprintf(lod, sizeof(lod), "%i^3", r->lod);
        char prepare[16];
        if (r->prepare_ms > 0) {
            snprintf(prepare, sizeof(prepare), "%.4f", r->prepare_ms);
        } else {
            snprintf(prepare, sizeof(prepare), "-");
        }
        zox_log("[tst] [metric] %-8s %-5s %-8s %6i %9s %9.4f %9.4f  %s", r->what, lod, r->how, r->faces, prepare, r->mesh_ms,
            r->prepare_ms + r->mesh_ms, r->note ? r->note : "");
    }
    zox_log("[tst]   prepare: the terrain chunk's sides built (sides) or its grid filled (grid); write: its faces written");
    zox_log("[tst]   terrain: its 6 neighbours at its lod; * its neighbours at full detail, a lod border on every side");
    zox_log("[tst] [metric] world far 16: [%i] chunks (%s) meshed once: sides [%.2f] ms, grid [%.2f] ms, %.1fx as fast",
        zox_tst_pm_world.chunks, zox_tst_pm_world_lods, zox_tst_pm_world.sides_ms, zox_tst_pm_world.grid_ms,
        zox_tst_pm_world.sides_ms / zox_tst_pm_world.grid_ms);
}

void zox_tst_perf_meshing_check(ecs* world) {
    (void) world;
    init_side_child_indices();
    zox_tst_pm_row_count = 0;
    // terrain: a chunk and its 6 neighbours, at each lod, sides then the grid
    const byte depth = terrain_depth;
    VoxelNode chunk, neighbor_trees[6];
    zox_tst_pm_terrain(&chunk, depth, int3_zero);
    const VoxelNode* neighbors[6];
    byte neighbor_depths[6];
    for (byte d = 0; d < 6; d++) {
        zox_tst_pm_terrain(&neighbor_trees[d], depth, move_position(int3_zero, d));
        neighbors[d] = &neighbor_trees[d];
        neighbor_depths[d] = depth;
    }
    zox_tst_pm_terrain_world w;
    zox_tst_pm_terrain_world_init(&w);
    // each lod with its neighbours at the same lod (most chunks: a ring of land at one lod), then the coarser lods
    //  beside neighbours drawn at full detail (where lods meet, every side here: the worst case)
    // a streamed world's chunks at each lod: columns in its circle (terrain_lod_far, int2_distance as the stream),
    //  each render_distance_y * 2 + 1 chunks, their lod as the stream picks it (camera_distance_to_terrain_render_depth)
    int lod_chunks[8] = { 0 };
    //  - the game's defaults (chunks3/set/lod.c: near 3, far 16), not the test's --tiny
    const byte far_lod = 16, near_lod = 3;
    for (int x = -far_lod; x <= far_lod; x++) for (int z = -far_lod; z <= far_lod; z++) {
        const int distance = int2_distance(int2_zero, (int2) { x, z });
        if (distance <= far_lod) {
            lod_chunks[camera_distance_to_render_depth(distance, depth, near_lod, far_lod)] += render_distance_y * 2 + 1;
        }
    }
    double lod_sides_ms[8] = { 0 }, lod_grid_ms[8] = { 0 };
    static char notes[8][64];
    int row = 0;
    for (int border = 0; border <= 1; border++) {
        for (int target = border ? depth - 1 : depth; target >= (border ? depth - 2 : 0); target--, row++) {
            const int lod = octree_size(target);
            for (byte d = 0; d < 6; d++) {
                neighbor_depths[d] = border ? depth : target;
            }
            double sides_prepare, sides_mesh, grid_prepare, grid_mesh;
            const int sides_faces = zox_tst_pm_textured(&w, &chunk, neighbors, neighbor_depths, target, 0, &sides_prepare, &sides_mesh);
            const int grid_faces = zox_tst_pm_textured(&w, &chunk, neighbors, neighbor_depths, target, 1, &grid_prepare, &grid_mesh);
            const int differ = zox_tst_pm_textured_differ(&w, &chunk, neighbors, neighbor_depths, target);
            snprintf(notes[row], 64, "%.1fx as fast as sides, %s", (sides_prepare + sides_mesh) / (grid_prepare + grid_mesh),
                differ == 0 ? "the same faces" : (differ < 0 ? "OTHER FACE COUNT" : "OTHER FACES"));
            const char* how_sides = border ? "sides*" : "sides";
            const char* how_grid = border ? "grid*" : "grid";
            zox_tst_pm_add("terrain", lod, how_sides, sides_faces, sides_prepare, sides_mesh, "the game's now");
            zox_tst_pm_add("terrain", lod, how_grid, grid_faces, grid_prepare, grid_mesh, notes[row]);
            if (!border) {
                lod_sides_ms[target] = sides_prepare + sides_mesh;
                lod_grid_ms[target] = grid_prepare + grid_mesh;
            }
            zox_tst_check(!differ, "terrain %i^3 (border %i): the grid's faces aren't sides' ([%i] differ, -1 a different count)",
                lod, border, differ);
        }
    }
    // the world: every chunk meshed once, each lod's time by its chunks
    double world_sides = 0, world_grid = 0;
    int world_chunks = 0;
    char lods_text[96] = "";
    for (int d = depth; d >= 0; d--) {
        world_sides += lod_chunks[d] * lod_sides_ms[d];
        world_grid += lod_chunks[d] * lod_grid_ms[d];
        world_chunks += lod_chunks[d];
        char part[24];
        snprintf(part, sizeof(part), "%s%i^3 %i", d == depth ? "" : ", ", octree_size(d), lod_chunks[d]);
        strncat(lods_text, part, sizeof(lods_text) - strlen(lods_text) - 1);
    }
    zox_tst_pm_world = (zox_tst_pm_world_mix) { world_chunks, world_sides, world_grid };
    snprintf(zox_tst_pm_world_lods, sizeof(zox_tst_pm_world_lods), "%s", lods_text);
    free(w.grid_buffer);
    collapse_VoxelNode(&chunk);
    for (byte d = 0; d < 6; d++) {
        collapse_VoxelNode(&neighbor_trees[d]);
    }
    // models: both walkers, at each lod
    VoxelNode model;
    zox_tst_pm_model(&model, zox_tst_pm_model_depth);
    color_rgb palette[3] = { { 200, 40, 40 }, { 40, 200, 40 }, { 40, 40, 200 } };
    const ColorRGBs colors = { .value = palette, .length = 3 };
    for (int pyramid = 1; pyramid >= 0; pyramid--) {
        for (int target = zox_tst_pm_model_depth; target >= zox_tst_pm_model_depth - 2; target--) {
            double ms;
            const int faces = zox_tst_pm_colored(&model, &colors, zox_tst_pm_model_depth, target, pyramid, &ms);
            zox_tst_pm_add("model", octree_size(target), pyramid ? "pyramid" : "walk", faces, 0, ms,
                pyramid ? "the game's for models up to 128^3" : "the game's for deeper models");
        }
    }
    collapse_VoxelNode(&model);
    zox_tst_pm_print();
}

void zox_tsts_chunks3_perf_meshing() {
    zox_tst_add("perf_meshing", zox_tst_perf_meshing_ready, zox_tst_perf_meshing_check, 120);
}
