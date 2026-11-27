// tst: zeland_boot, zeland starts a game (--autostart) and plays: no voxel terrain, no character, a free camera, no errors (the game's ui skips what needs a character)
//  - run: zox_tst_game=zeland bash bsh/tst.sh zeland_boot
byte zox_tst_zeland_boot_ready(ecs* world) {
    (void) world;
    zox_tst_waiting = "the game and its player to play";
    return zox_valid(local_game) && zox_getv(local_game, GameState) == zox_game_state_playing &&
        zox_getv(zox_players[0], PlayerState) == zox_player_state_playing;
}

void zox_tst_zeland_boot_check(ecs* world) {
    const entity player = zox_players[0];
    const entity realm = zox_get_link(world, local_game, RealmLink);
    const entity camera = zox_get_link(world, player, CameraLink);
    zox_tst_check(zox_valid(realm), "no realm");
    zox_tst_check(!zox_valid(zox_get_child_by_id(world, realm, zox_id(Terrain))), "a voxel terrain spawned");
    const entity terrain = zox_get_child_by_id(world, realm, zox_id(PolyTerrain));
    zox_tst_check(zox_valid(terrain) && zox_getv(terrain, MeshRenderCount) > 0, "the realm has no uploaded poly terrain");
    const entity character = zox_get_link(world, player, CharacterLink);
    zox_tst_check(zox_valid(character) && zox_get_link(world, character, ColliderLink), "no character with a capsule");
    zox_tst_check(zox_valid(camera) && zox_getv(camera, CameraState) == zox_camera_state_third_person &&
        zox_get_link(world, camera, CameraTarget) == character, "the camera isn't orbiting the character (state [%i] target [%lu] parent [%lu])",
        zox_valid(camera) ? zox_getv(camera, CameraState) : -1, zox_get_link(world, camera, CameraTarget), zox_get_parent(world, camera));
    const uint errors = zox_log_errors_count - zox_tst_errors_start;
    zox_tst_check(errors == 0, "[%u] errors logged", errors);
}


// tst: zeland_walk, auto jump on (as with a touchscreen), W held 2 seconds (the fake keyboard, as a player would): the
//  character walks away from the orbit camera, faces that way, on the ground; the camera follows without shaking
//  (the character still on screen frame to frame); a screenshot (zeland_walk.ppm)
double zox_tst_zw_time = 0;
float3 zox_tst_zw_start;
byte zox_tst_zw_shot = 0;
float3 zox_tst_zw_seen;         // the character's direction in the camera's frame, the frame before
float zox_tst_zw_shake = 0;     // the most it moved on screen in a frame while walking, degrees
float3 zox_tst_zw_forward;      // the camera's forward, the frame before
float zox_tst_zw_turn = 0;      // how far the camera turned the frame before, degrees
float zox_tst_zw_jerk = 0;      // the most its turn changed between frames: a shake turns it one way then the other
int zox_tst_zw_frames = 0;

static inline void zox_tst_zw_hold_w(ecs* world, byte held) {
    entity devices[zox_children_capacity];
    const uint length = zox_get_children_by_id(world, zox_players[0], devices, zox_children_capacity, zox_id(Device));
    for (uint i = 0; i < length; i++) {
        if (zox_has(devices[i], Keyboard)) {
            zox_set(devices[i], Keyboard, { .w.is_pressed = held });
        }
    }
}

// the character's direction from the camera, in the camera's own frame: where it is on screen
static inline float3 zox_tst_zw_on_screen(ecs* world, entity camera, entity character) {
    const float3 to = float3_normalize(float3_sub(zox_getv(character, Position3D), zox_getv(camera, Position3D)));
    return float4_rotate_float3(float4_inverse(zox_getv(camera, Rotation3D)), to);
}

byte zox_tst_zeland_walk_ready(ecs* world) {
    auto_jump = 1;     // on with a touchscreen: it must skip a poly terrain (it read its BlockScale, a crash)
    zox_tst_waiting = "the game to play with the orbit camera";
    if (!zox_tst_zeland_boot_ready(world)) {
        return 0;
    }
    const entity character = zox_get_link(world, zox_players[0], CharacterLink);
    const entity camera = zox_get_link(world, zox_players[0], CameraLink);
    if (!zox_valid(character) || zox_getv(camera, CameraState) != zox_camera_state_third_person ||
        !zox_getv(character, Grounded)) {
        return 0;
    }
    if (!zox_tst_zw_time) {
        zox_tst_zw_time = zox_current_time;
        zox_tst_zw_start = zox_getv(character, Position3D);
    }
    zox_tst_waiting = "the character to walk";
    if (zox_current_time - zox_tst_zw_time >= 2) {
        zox_tst_zw_hold_w(world, 0);
    }
    if (zox_current_time - zox_tst_zw_time < 2) {
        // the camera eased behind it after the first half second: from then it should hold the character still on screen
        const float3 seen = zox_tst_zw_on_screen(world, camera, character);
        if (zox_current_time - zox_tst_zw_time > 0.5) {
            const float turn = acosf(fminf(1, float3_dot(seen, zox_tst_zw_seen))) * 180 / M_PI;
            zox_tst_zw_shake = fmaxf(zox_tst_zw_shake, turn);
            zox_tst_zw_frames++;
        }
        zox_tst_zw_seen = seen;
        const float3 forward = float4_rotate_float3(zox_getv(camera, Rotation3D), (float3) { 0, 0, -1 });
        const float camera_turn = acosf(fminf(1, float3_dot(forward, zox_tst_zw_forward))) * 180 / M_PI;
        if (zox_current_time - zox_tst_zw_time > 0.5) {
            zox_tst_zw_jerk = fmaxf(zox_tst_zw_jerk, fabsf(camera_turn - zox_tst_zw_turn));
        }
        zox_tst_zw_turn = camera_turn;
        zox_tst_zw_forward = forward;
        // W held, as a player would (tests read no real keyboard: zox_keyboard_fake)
        zox_tst_zw_hold_w(world, 1);
        return 0;
    }
    if (!zox_tst_zw_shot) {
        zox_tst_zw_shot = 1;
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("zeland_walk.ppm"));
        return 0;
    }
    return !zox_screenshot_request[0];
}

void zox_tst_zeland_walk_check(ecs* world) {
    const entity character = zox_get_link(world, zox_players[0], CharacterLink);
    const entity camera = zox_get_link(world, zox_players[0], CameraLink);
    const float3 position = zox_getv(character, Position3D);
    const float3 moved = float3_sub(position, zox_tst_zw_start);
    // the camera starts at yaw 0, behind it on +z: forward is -z
    zox_tst_check(moved.z < -1 && fabsf(moved.x) < -moved.z / 3, "W didn't walk it away from the camera (moved [%f %f %f])", moved.x, moved.y, moved.z);
    const float3 facing = float4_rotate_float3(zox_getv(character, Rotation3D), (float3) { 0, 0, is_camera_positive_z ? 1 : -1 });
    zox_tst_check(facing.z < -0.9f, "it doesn't face where it walked (facing [%f %f %f])", facing.x, facing.y, facing.z);
    const entity terrain = zox_get_link(world, character, TerrainLink);
    const float3 local = float3_sub(position, zox_getv(terrain, Position3D));
    const float ground = poly_height_at(zox_get(terrain, PolyHeights)->value, zox_getv(terrain, PolyCells),
        zox_getv(terrain, Size2), local.x, local.z);
    zox_tst_check(position.y - 0.5f > ground - 0.05f, "it's under the ground");
    const float distance = float3_length(float3_sub(zox_getv(camera, Position3D), position));
    zox_tst_check(distance > 2 && distance < 12, "the camera didn't follow (distance [%f])", distance);
    entity meshes[8];
    const uint count = zox_get_children_by_id(world, character, meshes, 8, zox_id(VoxMesh));
    zox_tst_check(count == 1 && zox_getv(meshes[0], MeshRenderCount) > 0, "its capsule mesh isn't drawn ([%u] meshes, count [%i])",
        count, count ? (int) zox_getv(meshes[0], MeshRenderCount) : -1);
    zox_log("[tst] shake: the character moved on screen at most [%.3f] degrees a frame, the camera's turn changed at most [%.3f] between frames, over [%i] frames", zox_tst_zw_shake, zox_tst_zw_jerk, zox_tst_zw_frames);
    // walking straight on the gpu both are ~0.03; the play test's shake isn't reproduced here yet (2026-11-27)
    zox_tst_check(zox_tst_zw_frames > 5 && zox_tst_zw_shake < 0.15f, "the camera shakes: the character moved [%.3f] degrees on screen in a frame", zox_tst_zw_shake);
    zox_tst_check(zox_tst_zw_jerk < 0.15f, "the camera shakes: its turn changed [%.3f] degrees between frames", zox_tst_zw_jerk);
    const uint errors = zox_log_errors_count - zox_tst_errors_start;
    zox_tst_check(errors == 0, "[%u] errors logged", errors);
}

#include "views.c"
#include "move_feel.c"

void zox_tsts_zeland(ecs* world) {
    (void) world;
    zox_tst_add("zeland_boot", zox_tst_zeland_boot_ready, zox_tst_zeland_boot_check, 60);
    zox_tst_add("zeland_walk", zox_tst_zeland_walk_ready, zox_tst_zeland_walk_check, 60);
    zox_tsts_move_feel(world);
    zox_tst_add("zeland_player_views", zox_tst_zeland_player_views_ready, zox_tst_zeland_player_views_check, 60);
}
