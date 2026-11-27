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


// tst: zeland_walk, W pressed then pushed 2 seconds: the character walks away from the orbit camera (along its facing), turned that
//  way, on the ground; the camera follows; a screenshot (zeland_walk.ppm)
double zox_tst_zw_time = 0;
float3 zox_tst_zw_start;
byte zox_tst_zw_shot = 0;

static inline void zox_tst_zw_hold_w(ecs* world, byte held) {
    entity devices[zox_children_capacity];
    const uint length = zox_get_children_by_id(world, zox_players[0], devices, zox_children_capacity, zox_id(Device));
    for (uint i = 0; i < length; i++) {
        if (zox_has(devices[i], Keyboard)) {
            zox_set(devices[i], Keyboard, { .w.is_pressed = held });
        }
    }
}

byte zox_tst_zeland_walk_ready(ecs* world) {
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
    if (zox_current_time - zox_tst_zw_time < 2) {
        // W is read from SDL each frame (a test can't hold it): pressed once it turns the character to the camera's
        //  forward (movement.c), then it's pushed along its facing at walking speed (as tst auto_jump)
        zox_tst_zw_hold_w(world, 1);
        const float3 facing = float4_rotate_float3(zox_getv(character, Rotation3D), (float3) { 0, 0, -1 });
        const float3 velocity = zox_getv(character, Velocity3D);
        zox_setv(character, Velocity3D, ((float3) { facing.x * 3, velocity.y, facing.z * 3 }));
        return 0;
    }
    zox_tst_zw_hold_w(world, 0);
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
    zox_tst_check(moved.z < -1, "it didn't walk away from the camera (moved [%f %f %f])", moved.x, moved.y, moved.z);
    const float3 facing = float4_rotate_float3(zox_getv(character, Rotation3D), (float3) { 0, 0, -1 });
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
    const uint errors = zox_log_errors_count - zox_tst_errors_start;
    zox_tst_check(errors == 0, "[%u] errors logged", errors);
}

#include "views.c"

void zox_tsts_zeland(ecs* world) {
    (void) world;
    zox_tst_add("zeland_boot", zox_tst_zeland_boot_ready, zox_tst_zeland_boot_check, 60);
    zox_tst_add("zeland_walk", zox_tst_zeland_walk_ready, zox_tst_zeland_walk_check, 60);
    zox_tst_add("zeland_player_views", zox_tst_zeland_player_views_ready, zox_tst_zeland_player_views_check, 60);
}
