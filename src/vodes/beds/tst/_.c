// tst: bed_sleep, the player sleeps in a bed and wakes (vodes/beds), in a game (its player, camera and canvas)
//  - a bed placed (turned a quarter) with ground under it near the player; character_sleep, as a right click on it
//  - asleep: still (DisableMovement, DisableGravity, NoClip), lying over the bed's middle on the mattress, its up to
//    the pillow (the base's end), its front to the sky; a second later still there (no fall); its eyes shut
//    (CameraLids 1), the "Wake" window up, its mouse let go
//  - "Wake" (button_event_wake): moving again, standing upright beside the bed, out of its blocks, the eyes open,
//    the window gone, its mouse taken
//  - run: bash bsh/tst.sh bed_sleep
#define zox_tst_bsleep_direction 1
byte zox_tst_bsleep_stage = 0;
int zox_tst_bsleep_frames = 0;
double zox_tst_bsleep_time = 0;
int3 zox_tst_bsleep_base;
float3 zox_tst_bsleep_lying;

static inline entity zox_tst_bsleep_ui(ecs* world, entity player) {
    const entity canvas = zox_get_link(world, player, CanvasLink);
    return zox_valid(canvas) ? zox_get_child_by_id(world, canvas, zox_id(SleepUI)) : 0;
}

static inline byte zox_tst_bsleep_mouse_lock(ecs* world, entity player) {
    const entity mouse = zox_get_child_by_id(world, player, zox_id(Mouse));
    return zox_valid(mouse) ? zox_getv(mouse, MouseLock) : 255;
}

byte zox_tst_bed_sleep_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_tst_player_character(world);
    const entity player = zox_get_link(world, character, PlayerLink);
    const entity camera = zox_get_link(world, player, CameraLink);
    const entity terrain = zox_get_link(world, character, TerrainLink);
    const entity realm = zox_get_link(world, character, RealmLink);
    const float block = zox_getv(terrain, BlockScale);
    zox_tst_bsleep_frames++;
    zox_tst_progress(zox_tst_bsleep_stage * 1000 + zox_tst_bsleep_frames);
    if (zox_tst_bsleep_stage == 0) {
        zox_tst_waiting = "air for a bed";
        // air a block round it and two over, in the player's chunk column
        const int length = octree_size(zox_getv(terrain, NodeDepth));
        const int3 chunk = real_position_to_chunk_position(zox_getv(character, Position3D), length, block);
        for (int y = 0; y < length * 2 - 4; y++) {
            const int3 base = { chunk.x * length + 6, chunk.y * length + y, chunk.z * length + 8 };
            int solid = 0;
            for (int x = -2; x <= 2; x++) for (int h = 0; h < 3; h++) for (int z = -2; z <= 2; z++) {
                solid += terrain_voxel_at(world, terrain, (int3) { base.x + x, base.y + h, base.z + z }) != 0;
            }
            if (solid) {
                continue;
            }
            const byte bed = zox_getv(zox_tst_fplace_block(world, realm, zox_id(BlockBed)), BlockIndex);
            zox_tst_check(zox_tst_place_supported(world, terrain, realm, base, bed, zox_tst_bsleep_direction), "the bed wasn't placed");
            zox_tst_bsleep_base = base;
            zox_tst_bsleep_stage = 1;
            zox_tst_bsleep_frames = 0;
            return 0;
        }
        return zox_tst_bsleep_frames > 600;
    }
    if (zox_tst_bsleep_stage == 1) {
        zox_tst_waiting = "the bed placed";
        if (zox_tst_bsleep_frames < 10 || !terrain_voxel_at(world, terrain, zox_tst_bsleep_base)) {
            return zox_tst_bsleep_frames > 600;
        }
        character_sleep(world, character, zox_tst_bsleep_base);
        zox_tst_bsleep_stage = 2;
        zox_tst_bsleep_frames = 0;
        return 0;
    }
    if (zox_tst_bsleep_stage == 2) {
        if (zox_tst_bsleep_frames < 5) {
            return 0;
        }
        zox_tst_check(zox_has(character, Sleeping) && zox_has(character, DisableMovement) && zox_has(character, DisableGravity) &&
            zox_has(character, NoClip), "asleep it isn't still: Sleeping [%i] DisableMovement [%i] DisableGravity [%i] NoClip [%i]",
            zox_has(character, Sleeping), zox_has(character, DisableMovement), zox_has(character, DisableGravity), zox_has(character, NoClip));
        // over the bed's middle, on the mattress
        const int3 along = block_direction_offset(zox_tst_bsleep_direction, (int3) { 1, 0, 0 });
        const int3 b = zox_tst_bsleep_base;
        const float3 middle = { (b.x + 0.5f + 0.5f * along.x) * block, 0, (b.z + 0.5f + 0.5f * along.z) * block };
        const float3 p = zox_getv(character, Position3D);
        zox_tst_check(fabsf(p.x - middle.x) < 0.05f * block && fabsf(p.z - middle.z) < 0.05f * block,
            "asleep at [%.2f %.2f], not over the bed's middle [%.2f %.2f]", p.x, p.z, middle.x, middle.z);
        zox_tst_check(p.y > (b.y + bed_mattress_height) * block && p.y < (b.y + 1.5f) * block, "asleep at height [%.2f] blocks, not on the mattress",
            p.y / block - b.y);
        // its up to the pillow (its base's end), its front to the sky
        const float4 r = zox_getv(character, Rotation3D);
        const float3 up = float4_rotate_float3(r, (float3) { 0, 1, 0 });
        const float3 front = float4_rotate_float3(r, (float3) { 0, 0, 1 });
        zox_tst_check(up.x * -along.x + up.z * -along.z > 0.99f, "its head isn't to the pillow: up [%.2f %.2f %.2f]", up.x, up.y, up.z);
        zox_tst_check(front.y > 0.99f, "it isn't on its back: front [%.2f %.2f %.2f]", front.x, front.y, front.z);
        zox_tst_check(zox_valid(zox_tst_bsleep_ui(world, player)), "no Wake window");
        zox_tst_check(zox_tst_bsleep_mouse_lock(world, player) == 0, "its mouse is still locked");
        zox_tst_bsleep_lying = p;
        zox_tst_bsleep_stage = 3;
        zox_tst_bsleep_time = zox_current_time;
        return 0;
    }
    if (zox_tst_bsleep_stage == 3) {
        zox_tst_waiting = "asleep for 2 seconds";
        if (zox_current_time - zox_tst_bsleep_time < 2) {
            return 0;
        }
        const float moved = float3_distance(zox_getv(character, Position3D), zox_tst_bsleep_lying);
        const float3 p = zox_getv(character, Position3D), v = zox_getv(character, Velocity3D), a = zox_getv(character, Acceleration3D);
        zox_log("[tst] asleep moved [%.3f %.3f %.3f] velocity [%.3f %.3f %.3f] acceleration [%.3f %.3f %.3f]", p.x - zox_tst_bsleep_lying.x,
            p.y - zox_tst_bsleep_lying.y, p.z - zox_tst_bsleep_lying.z, v.x, v.y, v.z, a.x, a.y, a.z);
        zox_tst_check(moved < 0.01f * block, "asleep it moved [%.3f] blocks", moved / block);
        zox_tst_check(zox_getv(camera, CameraLids) >= 1, "asleep its eyes aren't shut [%.2f]", zox_getv(camera, CameraLids));
        button_event_wake(world, (ClickEventData) { .clicker = player });
        zox_tst_bsleep_stage = 4;
        zox_tst_bsleep_frames = 0;
        zox_tst_bsleep_time = zox_current_time;
        return 0;
    }
    if (zox_tst_bsleep_stage == 4) {
        // stood up (a few frames: before it can fall, the test's floor is only under the bed)
        if (zox_tst_bsleep_frames < 3) {
            return 0;
        }
        zox_tst_check(!zox_has(character, Sleeping) && !zox_has(character, DisableMovement) && !zox_has(character, DisableGravity) &&
            !zox_has(character, NoClip) && !zox_has(character, Waking), "awake it isn't moving again");
        const float3 up = float4_rotate_float3(zox_getv(character, Rotation3D), (float3) { 0, 1, 0 });
        zox_tst_check(up.y > 0.99f, "awake it isn't upright: up [%.2f %.2f %.2f]", up.x, up.y, up.z);
        const int3 at = real_position_to_block_position(zox_getv(character, Position3D), block);
        zox_tst_check(!block_is_solid_at(world, terrain, realm, at), "awake it's inside a solid block [%i] at [%i %i %i]",
            terrain_voxel_at(world, terrain, at), at.x, at.y, at.z);
        const float3 p = zox_getv(character, Position3D);
        const float across = sqrtf((p.x - zox_tst_bsleep_lying.x) * (p.x - zox_tst_bsleep_lying.x) + (p.z - zox_tst_bsleep_lying.z) * (p.z - zox_tst_bsleep_lying.z));
        zox_tst_check(across < 1.5f * block, "awake it's [%.2f] blocks from the bed", across / block);
        zox_tst_bsleep_stage = 5;
        return 0;
    }
    zox_tst_waiting = "awake, the eyes open";
    if (zox_current_time - zox_tst_bsleep_time < 1.5) {
        return 0;
    }
    zox_tst_check(zox_getv(camera, CameraLids) <= 0, "awake its eyes aren't open [%.2f]", zox_getv(camera, CameraLids));
    zox_tst_check(!zox_valid(zox_tst_bsleep_ui(world, player)), "the Wake window is still up");
    zox_tst_check(zox_tst_bsleep_mouse_lock(world, player) == 1, "its mouse isn't locked again");
    zox_tst_bsleep_stage = 6;
    return 1;
}

void zox_tst_bed_sleep_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_bsleep_stage == 6, "it stopped at stage [%i]", zox_tst_bsleep_stage);
}

void zox_tsts_beds(ecs* world) {
    zox_tst_add("bed_sleep", zox_tst_bed_sleep_ready, zox_tst_bed_sleep_check, 120);
}
