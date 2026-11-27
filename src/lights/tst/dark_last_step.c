// tst: dark_last_step, a dark reaching a voxel on its last step still looks round it: a glow stone beside it relights
//  it (lights3/sys/dark.c dark_flood_light), pure: its octrees built by hand, no world
//  - an octree of air, a glow stone at (2, 3, 2), the air under it lit by it; that air darkened as a dark's start with
//    its distance spent (0, as one crossing a chunk's edge arrives): the stone's light is queued back into it
//  - before: it returned at distance 0 without looking round it, nothing relit it, a home stayed dark once its door
//    shut (play test 2026-11-27; tst home_door_glow, a tool, finds it in the game)
//  - run: bash bsh/tst.sh dark_last_step
byte zox_tst_dark_last_step_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_dark_last_step_check(ecs* world) {
    (void) world;
    const byte depth = 2;
    const byte glow = 230;
    const byte3 stone = { 2, 3, 2 }, under = { 2, 2, 2 };
    VoxelNode voxels = { 0 };
    LightOctree lights = { .value = darklight };
    set_VoxelNode(&voxels, depth, stone, 1);       // voxel 1: the glow stone
    set_LightOctree(&lights, depth, under, darklight);  // darkened, as the dark's start does
    byte solidity[zox_light_solidity_length], glows[zox_light_solidity_length];
    memset(solidity, 0, sizeof(solidity));
    memset(glows, 0, sizeof(glows));
    solidity[0] = 1;
    glows[0] = glow;
    const VoxelNode* n_voxels[6] = { 0 };
    const ActiveOctree* n_actives[6] = { 0 };
    const LightOctree* n_lights[6] = { 0 };
    LightQueue* n_light_queues[6] = { 0 };
    DarkQueue* n_dark_queues[6] = { 0 };
    LightQueue light_queue;
    DarkQueue dark_queue;
    i_LightQueue(&light_queue);
    i_DarkQueue(&dark_queue);
    spinlock lock;
    spinlock_init(&lock);
    dark_flood_light(&lock, &voxels, NULL, &lights, n_voxels, n_actives, n_lights, n_light_queues, &light_queue,
        n_dark_queues, &dark_queue, depth, under, sunlight, 0, darklight, light_air_decay, solidity, glows);
    byte relit = 0;
    for (size_t i = 0; i < light_queue.count; i++) {
        const LightUpdate update = light_queue.ptr[i];
        relit |= byte3_equals(update.pos, under) && update.light == emitter_side_light(glow);
    }
    zox_tst_check(relit, "a dark on its last step beside a glow stone queued [%i] lights, none from the stone",
        (int) light_queue.count);
}
