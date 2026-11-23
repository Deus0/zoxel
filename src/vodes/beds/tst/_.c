// tst: bed_sleep, the player sleeps in a bed and wakes (vodes/beds), in a game (its player, camera and canvas)
//  - a bed placed (turned a quarter) with ground under it near the player; character_sleep, as a right click on it
//  - stood beside it, it climbs on: never over 8 blocks a second (no jumps), standing on the mattress half way
//  - asleep: still (DisableMovement, DisableGravity, NoClip), lying over the bed's middle on the mattress, its up to
//    the pillow (the base's end), its front to the sky; a second later still there (no fall); its eyes shut
//    (CameraLids 1), the "Wake" window up, its mouse let go
//  - "Wake" (button_event_wake): held still while it gets off (a climb's time, never over 8 blocks a second), then moving again, standing
//    upright beside the bed, out of its blocks; the eyes open, the window gone, its mouse taken
//  - run: bash bsh/tst.sh bed_sleep
#define zox_tst_bsleep_direction 1
byte zox_tst_bsleep_stage = 0;
int zox_tst_bsleep_frames = 0;
double zox_tst_bsleep_time = 0;
int3 zox_tst_bsleep_base;
float3 zox_tst_bsleep_lying, zox_tst_bsleep_last;
float zox_tst_bsleep_step = 0;   // its fastest climbing, blocks a second
byte zox_tst_bsleep_upright_middle = 0;   // seen standing on the mattress mid climb

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
        // beside it first, in reach, as a player clicking it stands
        const int3 side = int3_add(zox_tst_bsleep_base, block_direction_offset(zox_tst_bsleep_direction, (int3) { 0, 0, -1 }));
        const float3 bounds = zox_getv(character, Bounds3D);
        zox_setv(character, Position3D, ((float3) { (side.x + 0.5f) * block, side.y * block + bounds.y, (side.z + 0.5f) * block }));
        zox_setv(character, Velocity3D, float3_zero);
        character_sleep(world, character, zox_tst_bsleep_base);
        zox_tst_bsleep_stage = 2;
        zox_tst_bsleep_frames = 0;
        zox_tst_bsleep_time = zox_current_time;
        zox_tst_bsleep_last = zox_getv(character, Position3D);
        return 0;
    }
    if (zox_tst_bsleep_stage == 2) {
        zox_tst_waiting = "climbing onto the bed";
        const float3 now = zox_getv(character, Position3D);
        zox_tst_bsleep_step = float_max(zox_tst_bsleep_step, float3_distance(now, zox_tst_bsleep_last) / block / (float) zox_delta_time);
        zox_tst_bsleep_last = now;
        const float t = (float) (zox_current_time - zox_tst_bsleep_time) / sleep_climb_time;
        if (t > 0.45f && t < 0.55f) {
            const float3 up = float4_rotate_float3(zox_getv(character, Rotation3D), (float3) { 0, 1, 0 });
            zox_tst_bsleep_upright_middle |= up.y > 0.9f && now.y > (zox_tst_bsleep_base.y + bed_mattress_height) * block;
        }
        if (t < 1.2f) {
            return 0;
        }
        zox_log("[tst] climbed on, its fastest [%.2f] blocks a second, upright on the mattress mid way [%i]", zox_tst_bsleep_step,
            zox_tst_bsleep_upright_middle);
        zox_tst_check(zox_tst_bsleep_step < 8, "it moved [%.2f] blocks a second climbing on: a jump", zox_tst_bsleep_step);
        zox_tst_check(zox_tst_bsleep_upright_middle, "mid climb it wasn't standing on the mattress");
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
        zox_tst_check(moved < 0.01f * block, "asleep it moved [%.3f] blocks", moved / block);
        zox_tst_check(zox_getv(camera, CameraLids) >= 1, "asleep its eyes aren't shut [%.2f]", zox_getv(camera, CameraLids));
        button_event_wake(world, (ClickEventData) { .clicker = player });
        zox_tst_bsleep_stage = 4;
        zox_tst_bsleep_frames = 0;
        zox_tst_bsleep_step = 0;
        zox_tst_bsleep_last = zox_getv(character, Position3D);
        zox_tst_bsleep_time = zox_current_time;
        return 0;
    }
    if (zox_tst_bsleep_stage == 4) {
        // getting off: still held until off, then moving again, checked the frame it's off (before it can fall: the
        //  test's floor is only under the bed)
        zox_tst_waiting = "getting off the bed";
        const float3 now = zox_getv(character, Position3D);
        zox_tst_bsleep_step = float_max(zox_tst_bsleep_step, float3_distance(now, zox_tst_bsleep_last) / block / (float) zox_delta_time);
        zox_tst_bsleep_last = now;
        if (zox_has(character, SleepMotion)) {
            if (!zox_has(character, DisableMovement) && zox_tst_bsleep_frames > 2) {
                zox_tst_check(0, "getting off it can already move");
                return 1;
            }
            return zox_current_time - zox_tst_bsleep_time > 5;
        }
        zox_tst_check(zox_current_time - zox_tst_bsleep_time >= sleep_climb_time * 0.9f, "it was off in [%.2fs], under its climb",
            zox_current_time - zox_tst_bsleep_time);
        zox_log("[tst] got off, its fastest [%.2f] blocks a second", zox_tst_bsleep_step);
        zox_tst_check(zox_tst_bsleep_step < 8, "it moved [%.2f] blocks a second getting off: a jump", zox_tst_bsleep_step);
        zox_tst_check(!zox_has(character, Sleeping) && !zox_has(character, DisableMovement) && !zox_has(character, DisableGravity) &&
            !zox_has(character, NoClip) && !zox_has(character, Waking), "awake it isn't moving again");
        const float3 up = float4_rotate_float3(zox_getv(character, Rotation3D), (float3) { 0, 1, 0 });
        zox_tst_check(up.y > 0.99f, "awake it isn't upright: up [%.2f %.2f %.2f]", up.x, up.y, up.z);
        const int3 at = real_position_to_block_position(now, block);
        zox_tst_check(!block_is_solid_at(world, terrain, realm, at), "awake it's inside a solid block [%i] at [%i %i %i]",
            terrain_voxel_at(world, terrain, at), at.x, at.y, at.z);
        const float across = sqrtf((now.x - zox_tst_bsleep_lying.x) * (now.x - zox_tst_bsleep_lying.x) + (now.z - zox_tst_bsleep_lying.z) * (now.z - zox_tst_bsleep_lying.z));
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

// tst: bed_wake_pad, on a gamepad: asleep, its navigator selects "Wake" and A wakes it (it was stuck, nothing selected)
//  - a gamepad given to the player (zox_tst_spawn_pad), its mode gamepad; character_sleep where it stands
//  - the sleep window up: the gamepad's selection (its Target) in it; A pressed: awake (Sleeping gone)
//  - run: bash bsh/tst.sh bed_wake_pad
byte zox_tst_bwake_stage = 0;
int zox_tst_bwake_frames = 0;
byte zox_tst_bwake_selected = 0;

byte zox_tst_bed_wake_pad_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_tst_player_character(world);
    const entity player = zox_get_link(world, character, PlayerLink);
    const int f = ++zox_tst_bwake_frames;
    zox_tst_progress(zox_tst_bwake_stage * 1000 + f);
    memset(zox_tst_pad_held, 0, sizeof(zox_tst_pad_held));
    zox_tst_pad_stick = float2_zero;
    if (zox_tst_bwake_stage == 0) {
        zox_tst_waiting = "the gamepad's mode";
        if (!zox_valid(zox_tst_pad)) {
            zox_tst_pad = zox_tst_spawn_pad(world, player);
        }
        zox_tst_pad_stick = (float2) { 0, (f % 20) < 4 ? 0.5f : 0 };
        if (zox_getv(player, DeviceMode) == zox_device_mode_gamepad) {
            zox_tst_pad_stick = float2_zero;
            const entity terrain = zox_get_link(world, character, TerrainLink);
            character_sleep(world, character, real_position_to_block_position(zox_getv(character, Position3D), zox_getv(terrain, BlockScale)));
            zox_tst_bwake_stage = 1;
            zox_tst_bwake_frames = 0;
        }
        return f > 600;
    }
    if (zox_tst_bwake_stage == 1) {
        zox_tst_waiting = "the sleep window, its Wake selected";
        const entity ui = zox_tst_bsleep_ui(world, player);
        const entity target = zox_getv(zox_tst_pad, Target);
        if (zox_valid(ui) && zox_valid(target) && zox_get_parent_by_id(world, target, zox_id(SleepUI)) == ui) {
            zox_tst_bwake_selected = 1;
            zox_tst_bwake_stage = 2;
            zox_tst_bwake_frames = 0;
        }
        return f > 300;
    }
    zox_tst_waiting = "A to wake it";
    zox_tst_pad_held[zox_btn_a] = f >= 10 && f < 14;
    return (f > 14 && !zox_has(character, Sleeping)) || f > 300;
}

void zox_tst_bed_wake_pad_check(ecs* world) {
    const entity character = zox_tst_player_character(world);
    zox_log("[tst] bed_wake_pad stage [%i] wake selected [%i] asleep [%i]", zox_tst_bwake_stage, zox_tst_bwake_selected,
        zox_has(character, Sleeping));
    zox_tst_check(zox_tst_bwake_selected, "asleep, the gamepad didn't select Wake (stage [%i])", zox_tst_bwake_stage);
    zox_tst_check(zox_tst_bwake_stage == 2 && !zox_has(character, Sleeping), "A didn't wake it");
}

// tst: bed_respawn, a bed heals faster and is where its sleeper respawns
//  - health regenerates [sleep_regen_mul] times faster asleep than awake (each measured over 2s, from 1 health)
//  - asleep: its player's HomePosition set beside the bed, a toast says so
//  - woken, then killed: it respawns there (its new character within a block of it)
//  - run: bash bsh/tst.sh bed_respawn
byte zox_tst_brespawn_stage = 0;
double zox_tst_brespawn_time = 0;
float zox_tst_brespawn_awake = -1, zox_tst_brespawn_asleep = -1, zox_tst_brespawn_from = 0;
byte zox_tst_brespawn_toast = 0;
entity zox_tst_brespawn_dead = 0;
float zox_tst_brespawn_off = -1;        // the new character from its respawn point

static inline entity zox_tst_brespawn_health(ecs* world, entity character) {
    return zox_get_child_by_id(world, character, zox_id(StatHealth));
}

byte zox_tst_bed_respawn_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity player = dbg_player;
    const entity character = zox_get_link(world, player, CharacterLink);
    const entity health = zox_valid(character) ? zox_tst_brespawn_health(world, character) : 0;
    const double t = zox_current_time - zox_tst_brespawn_time;
    zox_tst_progress(zox_tst_brespawn_stage * 1000 + (int) (t * 10));
    switch (zox_tst_brespawn_stage) {
        case 0:
            zox_tst_waiting = "its health";
            if (!zox_valid(health) || zox_getv(player, PlayerState) != zox_player_state_playing) {
                return 0;
            }
            zox_setv(health, StatValue, 1);
            zox_tst_brespawn_from = 1;
            zox_tst_brespawn_time = zox_current_time;
            zox_tst_brespawn_stage = 1;
            return 0;
        case 1:
            zox_tst_waiting = "regenerating awake";
            if (t < 2) {
                return 0;
            }
            zox_tst_brespawn_awake = zox_getv(health, StatValue) - zox_tst_brespawn_from;
            zox_setv(health, StatValue, 1);
            {
                const entity terrain = zox_get_link(world, character, TerrainLink);
                character_sleep(world, character, real_position_to_block_position(zox_getv(character, Position3D),
                    zox_getv(terrain, BlockScale)));
            }
            zox_tst_brespawn_time = zox_current_time;
            zox_tst_brespawn_stage = 2;
            return 0;
        case 2:
            zox_tst_waiting = "regenerating asleep";
            if (!zox_tst_brespawn_toast) {
                const entity toast = zox_get_child_by_id(world, zox_get_link(world, player, CanvasLink), zox_id(Toast));
                zox_tst_brespawn_toast = zox_valid(toast);
            }
            if (t < 2) {
                return 0;
            }
            zox_tst_brespawn_asleep = zox_getv(health, StatValue) - zox_tst_brespawn_from;
            character_wake(world, character);
            zox_tst_brespawn_time = zox_current_time;
            zox_tst_brespawn_stage = 3;
            return 0;
        case 3:
            zox_tst_waiting = "awake and playing, then killed";
            if (zox_has(character, Sleeping) || zox_has(character, SleepMotion) ||
                zox_getv(player, PlayerState) != zox_player_state_playing) {
                return t > 10;
            }
            zox_tst_brespawn_dead = character;
            zox_setv(health, StatValue, 0);
            zox_tst_brespawn_time = zox_current_time;
            zox_tst_brespawn_stage = 4;
            return 0;
        default:
            zox_tst_waiting = "its respawn";
            if (zox_valid(character) && character != zox_tst_brespawn_dead && !zox_has(character, Dead) &&
                zox_getv(player, PlayerState) == zox_player_state_playing && zox_has(player, HomePosition)) {
                zox_tst_brespawn_off = float3_distance(zox_getv(character, Position3D), zox_getv(player, HomePosition));
                return 1;
            }
            return t > 30;
    }
}

void zox_tst_bed_respawn_check(ecs* world) {
    const entity player = dbg_player;
    zox_log("[tst] bed_respawn stage [%i] regen over 2s awake [%.2f] asleep [%.2f], toast [%i] home [%i], respawned [%.2f] from it",
        zox_tst_brespawn_stage, zox_tst_brespawn_awake, zox_tst_brespawn_asleep, zox_tst_brespawn_toast,
        zox_has(player, HomePosition), zox_tst_brespawn_off);
    zox_tst_check(zox_tst_brespawn_awake > 0, "it didn't regenerate awake [%.2f]", zox_tst_brespawn_awake);
    zox_tst_check(zox_tst_brespawn_asleep > zox_tst_brespawn_awake * (sleep_regen_mul - 1), "asleep [%.2f] isn't [%.0f] times awake [%.2f]",
        zox_tst_brespawn_asleep, sleep_regen_mul, zox_tst_brespawn_awake);
    zox_tst_check(zox_has(player, HomePosition), "sleeping didn't set its respawn point");
    zox_tst_check(zox_tst_brespawn_toast, "no toast for its respawn point");
    zox_tst_check(zox_tst_brespawn_off >= 0 && zox_tst_brespawn_off < 1, "it respawned [%.2f] from its bed, not beside it",
        zox_tst_brespawn_off);
}

void zox_tsts_beds(ecs* world) {
    zox_tst_add("bed_respawn", zox_tst_bed_respawn_ready, zox_tst_bed_respawn_check, 120);
    zox_tst_set_setup("bed_respawn", zox_tst_level_setup, 0);
    zox_tst_add("bed_wake_pad", zox_tst_bed_wake_pad_ready, zox_tst_bed_wake_pad_check, 120);
    zox_tst_set_setup("bed_wake_pad", zox_tst_level_setup, 0);
    zox_tst_add("bed_sleep", zox_tst_bed_sleep_ready, zox_tst_bed_sleep_check, 120);
}
