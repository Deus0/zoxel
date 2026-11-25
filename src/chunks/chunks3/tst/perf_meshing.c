// tst: perf_meshing, how long each cpu mesher takes a mesh, on the same voxels every run (doc/plans/chunks_refactor.md:
//  the meshers are merged, a change shouldn't make them slower)
//  - textured: a terrain chunk (terrain_depth, 16³) of hills, three blocks, caves, and its 6 neighbours, at each lod:
//      its sides (build_sides_dig) and its mesh (zox_terrain_building_dig) timed apart, both are what a chunk costs
//  - colored: a model (32³) a ball of 3 colours with holes, at each lod, both walkers (the pyramid grid, the octree walk)
//  - each the fastest of 5 rounds of a few builds (the least disturbed), in ms a mesh, and its faces: a mesher change
//    that draws other faces shows in the count (checked against the counts below)
//  - logs [tst] [metric] perf_meshing lines; compare a debug build's with a debug build's (doc/results.md has the last)
//  - run: bash bsh/tst.sh perf_meshing (--release for the speed that ships)
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

// the fastest of the rounds, ms a build
static inline double zox_tst_pm_fastest(const double* rounds) {
    double fastest = rounds[0];
    for (byte r = 1; r < zox_tst_pm_rounds; r++) {
        fastest = rounds[r] < fastest ? rounds[r] : fastest;
    }
    return fastest;
}

// faces each build made, by its name, to check meshes stay the same
typedef struct {
    const char* name;
    int faces;
} zox_tst_pm_count;

// textured at one lod: [sides ms, mesh ms], its faces
static int zox_tst_pm_textured(const VoxelNode* chunk, const VoxelNode** neighbors, const byte* neighbor_depths,
    byte depth, byte target, double* sides_ms, double* mesh_ms)
{
    // a tilemap's uvs for 3 blocks: each face its own tile, as a real tilemap
    float2 tile_uvs[3 * 24];
    for (int i = 0; i < 3 * 24; i++) {
        const int tile = i / 4;
        tile_uvs[i] = (float2) { (tile % 8) / 8.0f + ((i % 4) >= 2) / 8.0f, (tile / 8) / 8.0f + ((i % 4) == 1 || (i % 4) == 2) / 8.0f };
    }
    const TilemapUVs tilemap_uvs = { .value = tile_uvs, .length = 3 * 24 };
    byte solidity[255], heights[255];
    memset(solidity, 1, sizeof(solidity));
    memset(heights, 0, sizeof(heights));
    VoxelNodeLock lock;
    spinlock_init(&lock.value);
    const int builds = target >= 4 ? 10 : 40;
    double sides_rounds[zox_tst_pm_rounds], mesh_rounds[zox_tst_pm_rounds];
    int faces = 0;
    for (byte r = 0; r < zox_tst_pm_rounds; r++) {
        sides_rounds[r] = mesh_rounds[r] = 0;
        for (int b = 0; b < builds; b++) {
            SidesOctree sides;
            create_SidesOctree(&sides);
            double begin = get_time_ms();
            build_sides_dig(NULL, zox_block_material_solid, chunk, neighbors, neighbor_depths, chunk, &sides, target, 0, byte3_zero, 0);
            sides_rounds[r] += get_time_ms() - begin;
            mesh_uvs_build_data mesh = {
                .indicies = create_int_array_d(initial_dynamic_array_size),
                .vertices = create_float3_array_d(initial_dynamic_array_size),
                .uvs = create_float2_array_d(initial_dynamic_array_size),
                .color_rgbs = create_color_rgb_array_d(initial_dynamic_array_size)
            };
            const terrain_build_data data = {
                .tilemap_uvs = &tilemap_uvs,
                .voxel_solidity = solidity,
                .heights = heights,
                .mesh_data = &mesh,
            };
            begin = get_time_ms();
            zox_terrain_building_dig(data, &lock, &sides, chunk, byte3_zero, 8.0f, target, 0, 0);
            mesh_rounds[r] += get_time_ms() - begin;
            faces = mesh.vertices->size / 4;
            dispose_int_array_d(mesh.indicies);
            dispose_float3_array_d(mesh.vertices);
            dispose_float2_array_d(mesh.uvs);
            dispose_color_rgb_array_d(mesh.color_rgbs);
            collapse_SidesOctree(&sides);
        }
        sides_rounds[r] /= builds;
        mesh_rounds[r] /= builds;
    }
    *sides_ms = zox_tst_pm_fastest(sides_rounds);
    *mesh_ms = zox_tst_pm_fastest(mesh_rounds);
    (void) depth;
    return faces;
}

// colored at one lod, one walker: ms a mesh, its faces
static int zox_tst_pm_colored(const VoxelNode* model, const ColorRGBs* colors, byte depth, byte target, byte grid, double* ms) {
    const VoxelNode* noctrees[6] = { 0 };
    const byte nrdepths[6] = { 0 };
    const int builds = target >= 5 ? 5 : 20;
    double rounds[zox_tst_pm_rounds];
    int faces = 0;
    for (byte r = 0; r < zox_tst_pm_rounds; r++) {
        rounds[r] = 0;
        for (int b = 0; b < builds; b++) {
            mesh_colored_build_data mesh = {
                .indicies = create_int_array_d(initial_dynamic_array_size),
                .vertices = create_float3_array_d(initial_dynamic_array_size),
                .colors = create_color_rgb_array_d(initial_dynamic_array_size)
            };
            const double begin = get_time_ms();
            if (grid) {
                build_voxel_mesh_grid(model, colors, &mesh, depth, target, float3_zero, 1.0f, 1);
            } else {
                build_voxel_mesh_c(model, model, noctrees, nrdepths, colors, &mesh, target, 0, byte3_zero, float3_zero, 1.0f, 1);
            }
            rounds[r] += get_time_ms() - begin;
            faces = mesh.vertices->size / 4;
            dispose_int_array_d(mesh.indicies);
            dispose_float3_array_d(mesh.vertices);
            dispose_color_rgb_array_d(mesh.colors);
        }
        rounds[r] /= builds;
    }
    *ms = zox_tst_pm_fastest(rounds);
    return faces;
}

byte zox_tst_perf_meshing_ready(ecs* world) {
    (void) world;
    return 1;
}

// the faces each build made the first time this ran (2026-11-25): a mesher change that draws other faces fails here
static const zox_tst_pm_count zox_tst_pm_expected[] = {
    { "textured 4", 1206 }, { "textured 3", 202 }, { "textured 2", 81 },
    { "colored grid 5", 10022 }, { "colored grid 4", 5566 }, { "colored grid 3", 1798 },
    { "colored walk 5", 10022 }, { "colored walk 4", 5566 }, { "colored walk 3", 1798 },
};

static inline void zox_tst_pm_faces(const char* name, int faces) {
    for (uint i = 0; i < sizeof(zox_tst_pm_expected) / sizeof(zox_tst_pm_expected[0]); i++) {
        if (!strcmp(zox_tst_pm_expected[i].name, name) && zox_tst_pm_expected[i].faces >= 0) {
            zox_tst_check(faces == zox_tst_pm_expected[i].faces, "%s made [%i] faces, it made [%i] (the mesher's output changed)",
                name, faces, zox_tst_pm_expected[i].faces);
        }
    }
    zox_tst_check(faces > 0, "%s made no faces", name);
}

void zox_tst_perf_meshing_check(ecs* world) {
    (void) world;
    init_side_child_indices();
    // textured: a chunk and its 6 neighbours, at each lod
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
    char name[32];
    for (int target = depth; target >= depth - 2; target--) {
        double sides_ms, mesh_ms;
        const int faces = zox_tst_pm_textured(&chunk, neighbors, neighbor_depths, depth, target, &sides_ms, &mesh_ms);
        snprintf(name, sizeof(name), "textured %i", target);
        zox_log("[tst] [metric] perf_meshing %s: [%.4f] ms a mesh, sides [%.4f] mesh [%.4f], [%i] faces", name,
            sides_ms + mesh_ms, sides_ms, mesh_ms, faces);
        zox_tst_pm_faces(name, faces);
    }
    collapse_VoxelNode(&chunk);
    for (byte d = 0; d < 6; d++) {
        collapse_VoxelNode(&neighbor_trees[d]);
    }
    // colored: a model, both walkers, at each lod
    VoxelNode model;
    zox_tst_pm_model(&model, zox_tst_pm_model_depth);
    color_rgb palette[3] = { { 200, 40, 40 }, { 40, 200, 40 }, { 40, 40, 200 } };
    const ColorRGBs colors = { .value = palette, .length = 3 };
    for (int grid = 1; grid >= 0; grid--) {
        for (int target = zox_tst_pm_model_depth; target >= zox_tst_pm_model_depth - 2; target--) {
            double ms;
            const int faces = zox_tst_pm_colored(&model, &colors, zox_tst_pm_model_depth, target, grid, &ms);
            snprintf(name, sizeof(name), "colored %s %i", grid ? "grid" : "walk", target);
            zox_log("[tst] [metric] perf_meshing %s: [%.4f] ms a mesh, [%i] faces", name, ms, faces);
            zox_tst_pm_faces(name, faces);
        }
    }
    collapse_VoxelNode(&model);
}

void zox_tsts_chunks3_perf_meshing() {
    zox_tst_add("perf_meshing", zox_tst_perf_meshing_ready, zox_tst_perf_meshing_check, 120);
}
