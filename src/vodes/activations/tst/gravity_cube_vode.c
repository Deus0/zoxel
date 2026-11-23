// tst: gravity_cube_vode, a placed gravity cube's block vox plays its frames (vode_animate_system)
//  - in a game near its player (block voxes spawn only while one plays, as tst doors): a gravity cube two blocks
//    along +x of its feet, a block up
//  - its block's ModelLink (off) and ActiveModelLink (on) are VoxAnimations, their children its frames
//  - its block vox spawns (ModelAnimated); while off it shows more than one frame of its off animation (its lights
//    chasing), none of its on one; used (block_use, as a right click), within half a second a frame of its on one
//  - run: bash bsh/tst.sh gravity_cube_vode
byte zox_tst_gcv_stage = 0;
double zox_tst_gcv_time = 0, zox_tst_gcv_seen = 0;     // placed, its block vox first seen
int3 zox_tst_gcv_cube;
int zox_tst_gcv_off_frames = 0, zox_tst_gcv_off_wrong = 0;
byte zox_tst_gcv_animated = 0, zox_tst_gcv_on_seen = 0, zox_tst_gcv_animations = 0;

byte zox_tst_gravity_cube_vode_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_tst_player_character(world);
    const entity terrain = local_terrain;
    const entity realm = zox_get_parent(world, terrain);
    const float scale = zox_getv(terrain, BlockScale);
    const double t = zox_current_time - zox_tst_gcv_time;
    if (zox_tst_gcv_stage == 0) {
        const float3 p = zox_getv(character, Position3D);
        const float3 b = zox_getv(character, Bounds3D);
        const int3 feet = real_position_to_block_position((float3) { p.x, p.y - b.y + scale * 0.5f, p.z }, scale);
        zox_tst_gcv_cube = (int3) { feet.x + 2, feet.y + 1, feet.z };
        const byte cube = zox_getv(zox_get_child_by_id(world, realm, zox_id(BlockGravity)), BlockIndex);
        terrain_queue_voxel(world, terrain, zox_tst_gcv_cube, cube, 0);
        zox_tst_gcv_stage = 1;
        zox_tst_gcv_time = zox_current_time;
        return 0;
    }
    const entity kind = zox_get_child_by_id(world, realm, zox_id(BlockGravity));
    const entity off = zox_get_link(world, kind, ModelLink), on = zox_get_link(world, kind, ActiveModelLink);
    zox_tst_gcv_animations = zox_valid(off) && zox_has(off, VoxAnimation) && zox_valid(on) && zox_has(on, VoxAnimation) && off != on;
    const entity vode = terrain_vode_at(world, terrain, zox_tst_gcv_cube);
    const entity frame = zox_valid(vode) ? zox_get_link(world, vode, ModelLink) : 0;
    if (zox_tst_gcv_stage == 1) {
        zox_tst_waiting = "the cube's block vox to spawn and play its frames";
        if (!zox_valid(frame) || !zox_has(frame, ModelFrame)) {
            return t > 40;
        }
        zox_tst_gcv_animated = zox_has(vode, ModelAnimated);
        zox_tst_gcv_off_wrong += zox_get_parent(world, frame) != off;
        zox_tst_gcv_off_frames |= 1 << zox_getv(frame, ModelFrame);
        if (!zox_tst_gcv_seen) {
            zox_tst_gcv_seen = zox_current_time;
        }
        if (zox_current_time - zox_tst_gcv_seen < 2) {     // watched 2s from when it spawned
            return 0;
        }
        block_use(world, character, terrain, realm, zox_tst_gcv_cube);
        zox_tst_gcv_stage = 2;
        zox_tst_gcv_time = zox_current_time;
        return 0;
    }
    zox_tst_waiting = "the cube's on frames";
    if (zox_valid(frame) && t > 0.5) {
        zox_tst_gcv_on_seen = zox_get_parent(world, frame) == on && zox_has(frame, ModelActive);
        return 1;
    }
    return t > 3;
}

void zox_tst_gravity_cube_vode_check(ecs* world) {
    int frames_seen = 0;
    for (int f = 0; f < 8; f++) {
        frames_seen += (zox_tst_gcv_off_frames >> f) & 1;
    }
    zox_log("[tst] gravity_cube_vode: [%i] off frames shown, another animation's [%i] times while off, the on one's once on [%i]",
        frames_seen, zox_tst_gcv_off_wrong, zox_tst_gcv_on_seen);
    zox_tst_check(zox_tst_gcv_stage == 2, "the test stopped at stage [%i] (no block vox?)", zox_tst_gcv_stage);
    zox_tst_check(zox_tst_gcv_animations, "its block's ModelLink and ActiveModelLink aren't two VoxAnimations");
    zox_tst_check(zox_tst_gcv_animated, "its block vox isn't ModelAnimated");
    zox_tst_check(frames_seen >= 2, "its block vox showed [%i] frames while off, it isn't animated", frames_seen);
    zox_tst_check(!zox_tst_gcv_off_wrong, "its block vox showed another animation's frame [%i] times while off", zox_tst_gcv_off_wrong);
    zox_tst_check(zox_tst_gcv_on_seen, "its block vox didn't show its on animation's frames once on");
}
