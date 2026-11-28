#include "chunk3_parent.c"
#include "water_light.c"
#include "chunk_uvs.c"
#include "block_sides.c"
#include "glow.c"
#include "gpu.c"
#include "gpu_neighbours.c"
#include "dark_last_step.c"
// tst: smooth_lights, a face's vertices are lit by the air around them (lights3/sys/build_smooth.c)
//  - a vertex is the face's light, its two edges' and its corner's averaged (smooth_vertex_light)
//  - a corner behind two solid edges doesn't leak its light round them, it counts as darklight
//  - on real octrees: a floor voxel's up face, two solid blocks on its edges, bright air in the corner between
//  - past the chunks: the sky (sunlight) above, dark elsewhere
//  - a wall's face into a shut door: flat, the darkest open air round it (the room's), not the sunlit outside's
//  - run: bash bsh/tst.sh smooth_lights
// every face of every voxel at depth: the grid's vertex lights against the trees' (smooth_face_lights), mismatches
static int zox_tst_smooth_grid_mismatches(const LightOctree** lights, spinlock** lights_locks, const VoxelNode** voxels,
    spinlock** voxels_locks, const byte* solidity, byte depth)
{
    smooth_grid grid = { 0 };
    if (!smooth_grid_fill(&grid, lights, lights_locks, voxels, voxels_locks, depth)) {
        return -1;
    }
    int mismatches = 0;
    const byte length = octree_size(depth);
    for (byte x = 0; x < length; x++) for (byte y = 0; y < length; y++) for (byte z = 0; z < length; z++)
    for (byte direction = 0; direction < 6; direction++) {
        byte from_trees[4], from_grid[4];
        smooth_face_lights(lights, lights_locks, voxels, voxels_locks, solidity, (byte3) { x, y, z }, depth, direction, from_trees);
        smooth_face_lights_grid(&grid, grid.lights, solidity, (byte3) { x, y, z }, direction, from_grid);
        mismatches += memcmp(from_trees, from_grid, 4) != 0;
    }
    free(grid.lights);
    return mismatches;
}

byte zox_tst_smooth_lights_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_smooth_lights_check(ecs* world) {
    (void) world;
    // the rule
    zox_tst_check(smooth_vertex_light(100, 100, 100, 100, 0, 0) == 100, "open air isn't its light");
    zox_tst_check(smooth_vertex_light(100, 16, 100, 200, 1, 0) == (100 + 16 + 100 + 200) / 4, "one solid edge hid the corner");
    zox_tst_check(smooth_vertex_light(100, 16, 16, 255, 1, 1) == (100 + 16 + 16 + darklight) / 4, "two solid edges leaked the corner");
    // on octrees: the center chunk only, voxel 1 is solid
    const byte depth = 2;
    VoxelNode voxel_trees[27];
    LightOctree light_trees[27];
    const VoxelNode* voxels[27];
    const LightOctree* lights[27];
    spinlock lock_values[54];
    spinlock* voxels_locks[27];
    spinlock* lights_locks[27];
    const byte center = OCTREE_INDEX(0, 0, 0);
    for (byte i = 0; i < 27; i++) {
        voxels[i] = NULL;
        lights[i] = NULL;
        spinlock_init(&lock_values[i]);
        spinlock_init(&lock_values[27 + i]);
        voxels_locks[i] = &lock_values[i];
        lights_locks[i] = &lock_values[27 + i];
    }
    octree_create(&voxel_trees[center], sizeof(VoxelNode), 0);
    octree_create(&light_trees[center], sizeof(LightOctree), darklight);
    voxels[center] = &voxel_trees[center];
    lights[center] = &light_trees[center];
    byte solidity[zox_light_solidity_length];
    memset(solidity, 0, zox_light_solidity_length);
    solidity[0] = 1;
    VoxelNode* v = &voxel_trees[center];
    LightOctree* l = &light_trees[center];
    // the floor voxel at 1,1,1, its face looks up into 1,2,1
    set_VoxelNode(v, depth, (byte3) { 1, 1, 1 }, 1);
    // solid on the face's -x and -z edges, the corner between them (0,2,0) air and bright
    set_VoxelNode(v, depth, (byte3) { 0, 2, 1 }, 1);
    set_VoxelNode(v, depth, (byte3) { 1, 2, 0 }, 1);
    const byte3 lit[] = { { 1, 2, 1 }, { 2, 2, 1 }, { 1, 2, 2 }, { 2, 2, 2 }, { 2, 2, 0 }, { 0, 2, 2 } };
    for (byte i = 0; i < sizeof(lit) / sizeof(lit[0]); i++) {
        set_LightOctree(l, depth, lit[i], 100);
    }
    set_LightOctree(l, depth, (byte3) { 0, 2, 0 }, 255);
    byte vertex_lights[4];
    smooth_face_lights(lights, lights_locks, voxels, voxels_locks, solidity, (byte3) { 1, 1, 1 }, depth, direction_up, vertex_lights);
    zox_log("[tst] smooth up face vertices [%i] [%i] [%i] [%i]", vertex_lights[0], vertex_lights[1], vertex_lights[2], vertex_lights[3]);
    // --: both edges solid, the bright corner is hidden (it leaked: (100 + 16 + 16 + 255) / 4 = 96)
    zox_tst_check(vertex_lights[0] == (100 + darklight + darklight + darklight) / 4,
        "the corner behind two solids leaked, vertex [%i]", vertex_lights[0]);
    // -+ and +-: one solid edge, the corner is seen
    zox_tst_check(vertex_lights[1] == (100 + darklight + 100 + 100) / 4, "vertex -+ is [%i]", vertex_lights[1]);
    zox_tst_check(vertex_lights[3] == (100 + 100 + darklight + 100) / 4, "vertex +- is [%i]", vertex_lights[3]);
    // ++: open air all round
    zox_tst_check(vertex_lights[2] == 100, "vertex ++ is [%i]", vertex_lights[2]);
    // the chunk's top: the sky above, past it
    set_VoxelNode(v, depth, (byte3) { 3, 3, 3 }, 1);
    smooth_face_lights(lights, lights_locks, voxels, voxels_locks, solidity, (byte3) { 3, 3, 3 }, depth, direction_up, vertex_lights);
    zox_tst_check(vertex_lights[0] == sunlight && vertex_lights[2] == sunlight, "the sky over the chunk's top isn't sunlight [%i]", vertex_lights[0]);
    // a wall's face into a shut door (voxel 2, light_solidity_open), its filler over it, the floor under: the outside
    //  (-x) sunlit, the room (+x) dark; flat, the room's light (its -x vertices averaged the outside in, lit by a door)
    solidity[1] = light_solidity_open;
    set_VoxelNode(v, depth, (byte3) { 2, 1, 2 }, 1);
    set_VoxelNode(v, depth, (byte3) { 2, 1, 3 }, 2);
    set_VoxelNode(v, depth, (byte3) { 2, 2, 3 }, 2);
    for (byte x = 1; x <= 3; x++) {
        set_VoxelNode(v, depth, (byte3) { x, 0, 3 }, 1);
    }
    set_LightOctree(l, depth, (byte3) { 1, 1, 3 }, 200);
    set_LightOctree(l, depth, (byte3) { 1, 2, 3 }, 200);
    smooth_face_lights(lights, lights_locks, voxels, voxels_locks, solidity, (byte3) { 2, 1, 2 }, depth, direction_front, vertex_lights);
    zox_log("[tst] a wall's face into a shut door [%i] [%i] [%i] [%i]", vertex_lights[0], vertex_lights[1], vertex_lights[2], vertex_lights[3]);
    for (byte i = 0; i < 4; i++) {
        zox_tst_check(vertex_lights[i] == darklight, "a wall's face into a shut door took the outside's light, vertex [%i] [%i]", i,
            vertex_lights[i]);
    }
    // the room lit too: its light
    set_LightOctree(l, depth, (byte3) { 3, 1, 3 }, 120);
    set_LightOctree(l, depth, (byte3) { 3, 2, 3 }, 120);
    smooth_face_lights(lights, lights_locks, voxels, voxels_locks, solidity, (byte3) { 2, 1, 2 }, depth, direction_front, vertex_lights);
    zox_tst_check(vertex_lights[0] == 120 && vertex_lights[2] == 120, "a wall's face into a door, both sides lit, is [%i] not [120]",
        vertex_lights[0]);
    // the flattened grid the meshes light from gives the same lights, every face
    const int mismatches = zox_tst_smooth_grid_mismatches(lights, lights_locks, voxels, voxels_locks, solidity, depth);
    zox_tst_check(!mismatches, "[%i] faces light differently from the grid than from the trees", mismatches);
    close_VoxelNode(v);
    close_LightOctree(l);
}

// tst: lod_lights, a coarse lod mesh reads lights from a detailed light tree (dark faces in the distance)
//  - an open light node's own value is stale (set when it split), a coarse read gives its leaves' max
//  - in a chunk, and across into a neighbor chunk, and smooth lighting at a coarse depth isn't dark
//  - run: bash bsh/tst.sh lod_lights
void zox_tst_lod_lights_check(ecs* world) {
    // A corner light sample follows three orthogonal links, even when this is its only loaded path.
    entity path[4];
    for (int i = 0; i < 4; i++) {
        path[i] = zox_new();
        zox_set(path[i], ChunkNeighbors, { 0 });
    }
    const byte directions[] = { direction_right, direction_up, direction_front };
    sbyte3 offset = sbyte3_zero;
    for (int i = 0; i < 3; i++) {
        zox_get_mut(path[i], ChunkNeighbors)->value[directions[i]] = path[i + 1];
        offset = sbyte3_add(offset, neighbor_offsets[directions[i]]);
    }
    entity nearby[27];
    fetch_nearby_chunks(world, path[0], zox_getv(path[0], ChunkNeighbors), nearby);
    zox_tst_check(nearby[OCTREE_INDEX3(offset)] == path[3], "the diagonal chunk's light is missing from the grid");
    for (int i = 0; i < 4; i++) {
        zox_delete(path[i]);
    }
    // A shaded neighbour unloads and later returns with a different light.
    byte levels[27] = { 0 }, blocks[27] = { 0 };
    smooth_grid edge = { .length = 3, .lights = levels, .voxels = blocks };
    GridBorderSamples border = { 0 };
    const byte left = OCTREE_INDEX(-1, 0, 0);
    edge.present[left] = edge.voxels_present[left] = 1;
    levels[4] = 75;
    blocks[4] = 3;
    smooth_grid_keep_border(&edge, &border, 0);
    edge.present[left] = edge.voxels_present[left] = 0;
    levels[4] = blocks[4] = 0;
    smooth_grid_keep_border(&edge, &border, 1u << left);
    zox_tst_check(edge.present[left] && levels[4] == 75 && blocks[4] == 3 && !edge.voxels_present[left],
        "unloaded border changed its shade or remained present for face culling");
    edge.voxels_present[left] = 1;
    levels[4] = 190;
    smooth_grid_keep_border(&edge, &border, 1u << left);
    edge.present[left] = edge.voxels_present[left] = 0;
    levels[4] = 0;
    smooth_grid_keep_border(&edge, &border, 1u << left);
    zox_tst_check(levels[4] == 190, "the returning neighbour did not refresh its border light");
    resize_GridBorderSamples(&border, 0);
    const byte detail = 2, coarse = 1;
    VoxelNode voxel_trees[27];
    LightOctree light_trees[27];
    const VoxelNode* voxels[27];
    const LightOctree* lights[27];
    spinlock lock_values[54];
    spinlock* voxels_locks[27];
    spinlock* lights_locks[27];
    for (byte i = 0; i < 27; i++) {
        voxels[i] = NULL;
        lights[i] = NULL;
        spinlock_init(&lock_values[i]);
        spinlock_init(&lock_values[27 + i]);
        voxels_locks[i] = &lock_values[i];
        lights_locks[i] = &lock_values[27 + i];
    }
    const byte center = OCTREE_INDEX(0, 0, 0), right = OCTREE_INDEX(1, 0, 0);
    const byte chunks[2] = { center, right };
    for (byte c = 0; c < 2; c++) {
        octree_create(&voxel_trees[chunks[c]], sizeof(VoxelNode), 0);
        octree_create(&light_trees[chunks[c]], sizeof(LightOctree), darklight);
        voxels[chunks[c]] = &voxel_trees[chunks[c]];
        lights[chunks[c]] = &light_trees[chunks[c]];
    }
    byte solidity[zox_light_solidity_length];
    memset(solidity, 0, zox_light_solidity_length);
    solidity[0] = 1;
    // a coarse solid voxel (0,0,0) at depth 1, the air above it lit only in its detailed leaves
    set_VoxelNode(&voxel_trees[center], coarse, byte3_zero, 1);
    for (byte x = 0; x < 2; x++) for (byte y = 2; y < 4; y++) for (byte z = 0; z < 2; z++) {
        set_LightOctree(&light_trees[center], detail, (byte3) { x, y, z }, 200);
    }
    // its neighbor across +x: lit leaves beside the coarse voxel (1,0,0) at depth 1
    set_VoxelNode(&voxel_trees[center], coarse, (byte3) { 1, 0, 0 }, 1);
    for (byte y = 0; y < 2; y++) for (byte z = 0; z < 2; z++) {
        set_LightOctree(&light_trees[right], detail, (byte3) { 0, y, z }, 150);
    }
    const byte stale = getv_nearby_LightOctree(lights, lights_locks, byte3_zero, coarse, (sbyte3) { 0, 1, 0 }, darklight);
    const byte up = getv_nearby_max_LightOctree(lights, lights_locks, byte3_zero, coarse, (sbyte3) { 0, 1, 0 }, darklight);
    const byte across = getv_nearby_max_LightOctree(lights, lights_locks, (byte3) { 1, 0, 0 }, coarse, (sbyte3) { 1, 0, 0 }, darklight);
    zox_log("[tst] coarse read: stale [%i] leaf max [%i], across chunks [%i]", stale, up, across);
    zox_tst_check(stale != 200, "the open node held its leaves' light, the test can't see the stale read");
    zox_tst_check(up == 200, "a coarse read above lit leaves gave [%i]", up);
    zox_tst_check(across == 150, "a coarse read into the neighbor chunk's lit leaves gave [%i]", across);
    // smooth lighting at the coarse depth: the up face isn't dark
    byte vertex_lights[4];
    smooth_face_lights(lights, lights_locks, voxels, voxels_locks, solidity, byte3_zero, coarse, direction_up, vertex_lights);
    zox_log("[tst] coarse up face vertices [%i] [%i] [%i] [%i]", vertex_lights[0], vertex_lights[1], vertex_lights[2], vertex_lights[3]);
    for (byte v = 0; v < 4; v++) {
        zox_tst_check(vertex_lights[v] > darklight * 2, "coarse vertex [%i] is dark [%i]", v, vertex_lights[v]);
    }
    // the grid at the coarse depth and at the detailed one, across the two chunks
    const int coarse_mismatches = zox_tst_smooth_grid_mismatches(lights, lights_locks, voxels, voxels_locks, solidity, coarse);
    const int detail_mismatches = zox_tst_smooth_grid_mismatches(lights, lights_locks, voxels, voxels_locks, solidity, detail);
    zox_tst_check(!coarse_mismatches && !detail_mismatches, "the grid lights [%i] coarse and [%i] detailed faces differently from the trees",
        coarse_mismatches, detail_mismatches);
    // A settled neighbour drawing its root can retain finer voxels. The GPU border must sample the drawn root.
    set_VoxelNode(&voxel_trees[right], detail, byte3_zero, 1);
    voxel_trees[right].value = 2;
    smooth_grid grid = { .use_neighbor_depths = 1 };
    if (smooth_grid_fill(&grid, lights, lights_locks, voxels, voxels_locks, detail)) {
        const byte across_root = smooth_grid_voxel(&grid, (byte3) { 3, 0, 0 }, (sbyte3) { 1, 0, 0 });
        zox_tst_check(across_root == 2, "depth-zero border read voxel [%i], expected the root [2]", across_root);
        free(grid.lights);
    } else {
        zox_tst_check(0, "could not build the depth-zero border grid");
    }
    for (byte c = 0; c < 2; c++) {
        close_VoxelNode(&voxel_trees[chunks[c]]);
        close_LightOctree(&light_trees[chunks[c]]);
    }
}

void zox_tsts_lights() {
    zox_tst_add("dark_last_step", zox_tst_dark_last_step_ready, zox_tst_dark_last_step_check, 10);
    zox_tst_add("water_light", zox_tst_water_light_ready, zox_tst_water_light_check, 60);
    zox_tst_set_setup("water_light", NULL, 1);
    zox_tst_add("chunk_uvs", zox_tst_chunk_uvs_ready, zox_tst_chunk_uvs_check, 60);
    zox_tst_set_setup("chunk_uvs", zox_tst_chunk_uvs_setup, 1);
    zox_tst_add("block_sides", zox_tst_block_sides_ready, zox_tst_block_sides_check, 60);
    zox_tst_set_setup("block_sides", NULL, 1);
    zox_tst_add("glow_view", zox_tst_glow_view_ready, zox_tst_glow_view_check, 60);
    zox_tst_set_setup("glow_view", NULL, 1);
    zox_tst_add("lod_lights", zox_tst_smooth_lights_ready, zox_tst_lod_lights_check, 30);
    zox_tst_add("smooth_lights", zox_tst_smooth_lights_ready, zox_tst_smooth_lights_check, 30);
}
