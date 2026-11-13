// tst: smooth_lights, a face's vertices are lit by the air around them (lights3/sys/build_smooth.c)
//  - a vertex is the face's light, its two edges' and its corner's averaged (smooth_vertex_light)
//  - a corner behind two solid edges doesn't leak its light round them, it counts as darklight
//  - on real octrees: a floor voxel's up face, two solid blocks on its edges, bright air in the corner between
//  - past the chunks: the sky (sunlight) above, dark elsewhere
//  - run: bash bsh/tst.sh smooth_lights
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
    LightNode light_trees[27];
    const VoxelNode* voxels[27];
    const LightNode* lights[27];
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
    octree_create(&light_trees[center], sizeof(LightNode), darklight);
    voxels[center] = &voxel_trees[center];
    lights[center] = &light_trees[center];
    byte solidity[zox_light_solidity_length];
    memset(solidity, 0, zox_light_solidity_length);
    solidity[0] = 1;
    VoxelNode* v = &voxel_trees[center];
    LightNode* l = &light_trees[center];
    // the floor voxel at 1,1,1, its face looks up into 1,2,1
    set_VoxelNode(v, depth, (byte3) { 1, 1, 1 }, 1);
    // solid on the face's -x and -z edges, the corner between them (0,2,0) air and bright
    set_VoxelNode(v, depth, (byte3) { 0, 2, 1 }, 1);
    set_VoxelNode(v, depth, (byte3) { 1, 2, 0 }, 1);
    const byte3 lit[] = { { 1, 2, 1 }, { 2, 2, 1 }, { 1, 2, 2 }, { 2, 2, 2 }, { 2, 2, 0 }, { 0, 2, 2 } };
    for (byte i = 0; i < sizeof(lit) / sizeof(lit[0]); i++) {
        set_LightNode(l, depth, lit[i], 100);
    }
    set_LightNode(l, depth, (byte3) { 0, 2, 0 }, 255);
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
    close_VoxelNode(v);
    close_LightNode(l);
}

void zox_tsts_lights() {
    zox_tst_add("smooth_lights", zox_tst_smooth_lights_ready, zox_tst_smooth_lights_check, 30);
}
