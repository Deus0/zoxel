// tst: coop_block_crack, split screen: a block each player hits shows its crack overlay (block_damage_queue_system's
//  cube, shader_basic3_overlay) on their screen
//  - both look down at the ground, each hits the block its ray is on (0.8 of its least health, it doesn't break)
//  - a crack cube spawns at each block; screenshots tst/coop_block_crack.ppm (both halves)
//  - play test 2026-11-20: the second player saw no crack
//  - run: bash bsh/tst.sh coop_block_crack
extern char zox_screenshot_request[256];
int zox_tst_crack_stage = 0;
int zox_tst_crack_frames = 0;
int zox_tst_crack_cubes = 0;

static inline int zox_tst_crack_count(ecs* world) {
    int count = 0;
    ecs_iter_t it = ecs_each_id(world, zox_id(MeshBasic3));
    while (ecs_each_next(&it)) {
        for (int i = 0; i < it.count; i++) {
            const entity e = it.entities[i];
            if (zox_get_link(world, e, ShaderLink) != shader_basic3_overlay) {
                continue;
            }
            count++;
            const float3 p = zox_getv(e, Position3D);
            const color c = zox_getv(e, Color);
            zox_log("[tst] crack cube at [%.2f %.2f %.2f] alpha [%.2f] colour [%i %i %i %i] render disabled [%i] indicies [%i] mesh [%i]",
                p.x, p.y, p.z, zox_getv(e, Alpha), c.r, c.g, c.b, c.a, zox_getv(e, RenderDisabled), zox_get(e, MeshIndicies)->length,
                zox_getv(e, MeshGPULink).x);
        }
    }
    return count;
}

byte zox_tst_coop_block_crack_ready(ecs* world) {
    if (!zox_tst_coop_playing(world)) {
        return 0;
    }
    zox_tst_crack_frames++;
    zox_tst_progress(zox_tst_crack_stage * 1000 + zox_tst_crack_frames);
    for (int p = 0; p < 2; p++) {
        const entity character = zox_get_link(world, zox_players[p], CharacterLink);
        const entity skeleton = zox_get_link(world, character, SkeletonLink);
        const entity head = zox_valid(skeleton) ? zox_get_link(world, skeleton, HeadBoneLink) : 0;
        const entity camera = zox_valid(head) ? zox_get_child_by_id(world, head, zox_id(Camera)) : 0;
        if (zox_valid(camera)) {
            // looking down at the ground, the sign that looks down (as tst guard_pitch)
            float3 euler = quaternion_to_euler(zox_getv(camera, LocalRotation3D));
            const float3 ahead = float4_rotate_float3(euler_to_quaternion((float3) { 0.5f, euler.y, euler.z }), (float3) { 0, 0, -1 });
            euler.x = ahead.y < 0 ? 1.0f : -1.0f;
            zox_setv(camera, LocalRotation3D, euler_to_quaternion(euler));
        }
    }
    if (zox_tst_crack_stage == 0) {
        zox_tst_waiting = "both looking down, their rays on the ground";
        if (zox_tst_crack_frames < 60) {
            return 0;
        }
        int hit = 0;
        for (int p = 0; p < 2; p++) {
            const entity character = zox_get_link(world, zox_players[p], CharacterLink);
            const RaycastVoxelData* ray = zox_has(character, RaycastVoxelData) ? zox_get(character, RaycastVoxelData) : NULL;
            if (ray && ray->result == rayhit_terrain && zox_valid(ray->chunk)) {
                // as damage_terrain_block (skills, after huds) queues it
                zox_muter(ray->chunk, BlockDamageQueue, queue);
                const float least = zox_valid(ray->hit_block) && zox_has(ray->hit_block, BlockHealth) ? zox_getv(ray->hit_block, BlockHealth).x : 1;
                a_BlockDamageQueue(queue, (BlockDamageUpdate) { .position = ray->positionl, .damage = 0.8f * least });
                hit++;
            }
            zox_log("[tst] player [%i]'s ray [%i]", p + 1, ray ? ray->result : -1);
        }
        if (hit < 2) {
            return zox_tst_crack_frames > 600;
        }
        zox_tst_crack_stage = 1;
        zox_tst_crack_frames = 0;
        return 0;
    }
    zox_tst_waiting = "the cracks, a screenshot";
    if (zox_tst_crack_frames == 20) {
        zox_tst_crack_cubes = zox_tst_crack_count(world);
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("coop_block_crack.ppm"));
    }
    return zox_tst_crack_frames > 30;
}

void zox_tst_coop_block_crack_check(ecs* world) {
    (void) world;
    zox_log("[tst] coop_block_crack: [%i] crack cubes", zox_tst_crack_cubes);
    zox_tst_check(zox_tst_crack_stage == 1, "the players' rays never both hit the ground");
    zox_tst_check(zox_tst_crack_cubes >= 2, "[%i] crack cubes for two blocks hit", zox_tst_crack_cubes);
}
