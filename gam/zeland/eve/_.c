
// the game's state changed (its core Dirty): at boot, the start menu
void game_state_zeland(iter* it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GameState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(GameState, state);
        if (state->value == zox_game_boot) {
            spawn_boot_menu(world);
        }
    }
}

// the character's look: a capsule mesh child, lit, its front darker (geometry/fun/capsule.c)
static inline entity zeland_spawn_capsule_mesh(ecs* world, entity character, float radius, float height) {
    MeshVertices vertices = { 0 };
    MeshIndicies indices = { 0 };
    MeshColors colors = { 0 };
    spinlock_init(&vertices.lock);
    spinlock_init(&indices.lock);
    spinlock_init(&colors.lock);
    build_capsule_mesh(&vertices, &indices, &colors, radius, height, 16, (color_rgb) { 230, 120, 60 });
    const entity e = zox_new();
    zox_add(e, Mesh);
    zox_add(e, VoxMesh);
    zox_setv(e, Position3D, float3_zero);
    zox_setv(e, Rotation3D, quaternion_identity);
    zox_setv(e, Scale1, 1);
    zox_setv(e, LocalPosition3D, float3_zero);
    zox_setv(e, LocalRotation3D, quaternion_identity);
    zox_setv(e, TransformMatrix, float4x4_identity);
    zox_setv(e, RenderDisabled, 0);
    zox_setv(e, Brightness, 1);
    zox_setv(e, MeshGPULink, guint2_zero);
    zox_setv(e, ColorsGPULink, 0);
    zox_setv(e, MeshRenderCount, 0);
    zox_set_ptr(e, MeshVertices, vertices);
    zox_set_ptr(e, MeshIndicies, indices);
    zox_set_ptr(e, MeshColors, colors);
    dispose_MeshVertices(&vertices);
    dispose_MeshIndicies(&indices);
    dispose_MeshColors(&colors);
    zox_add(e, MeshDirty);
    zox_add(e, PreInitialize);  // its gpu buffers made, then uploaded
    zox_set_parent(world, e, character);
    return e;
}

// once playing, its camera orbits its character (third person): spawning attached it first person to it, undone here
static inline void zeland_orbit_camera(ecs* world, entity e) {
    const entity camera = zox_get_link(world, e, CameraLink);
    const entity character = zox_get_link(world, e, CharacterLink);
    if (!zox_valid(camera) || !zox_valid(character)) {
        return;
    }
    zox_set_parent(world, camera, 0);
    zox_remove(camera, LocalPosition3D);
    zox_remove(camera, LocalRotation3D);
    zox_remove(camera, Roaming);
    zox_remove(camera, CanRoam);
    camera_follow_mode = zox_camera_follow_mode_orbit;
    zox_set_link(world, camera, CameraTarget, character);
    zox_setv(camera, OrbitDistance, orbit_distance);
    zox_setv(camera, CameraState, zox_camera_state_third_person);
    zox_setv(camera, FieldOfView, get_camera_state_fov(zox_camera_state_third_person));
    zox_setv(camera, Euler, ((float3) { 0.35f, 0, 0 }));
    zox_setv(camera, Position3D, float3_add(zox_getv(character, Position3D), (float3) { 0, 3, 6 }));
    if (local_mouse) {
        zox_setv(local_mouse, MouseLock, 1);
    }
}

// a player loading into a game: its character on the island's middle (a capsule), the orbit camera on it, then plays
void player_state_zeland(iter* it) {
    const float radius = 0.35f;
    const float height = 1.0f;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerState, state);
        if (state->value == zox_player_state_play_begin) {
            zeland_orbit_camera(world, e);
        }
        if (state->value != zox_player_state_loading) {
            continue;
        }
        const entity camera = zox_get_link(world, e, CameraLink);
        const entity realm = zox_get_link(world, zox_get_parent(world, e), RealmLink);
        const entity terrain = zox_valid(realm) ? zox_get_child_by_id(world, realm, zox_id(PolyTerrain)) : 0;
        if (!zox_valid(camera) || !zox_valid(terrain)) {
            zox_loge("[zeland] no camera or poly terrain to start on");
            continue;
        }
        const PolyHeights* heights = zox_get(terrain, PolyHeights);
        const float2 size = zox_getv(terrain, Size2);
        // from the peak south (+z) to the first low ground (under 40% of the hills' height): it faces the mountain
        float z = size.y / 2;
        float ground = 0;
        for (; z < size.y; z += 1) {
            ground = poly_height_at(heights->value, zox_getv(terrain, PolyCells), size, size.x / 2, z);
            if (ground < poly_terrain_height * 0.4f) {
                break;
            }
        }
        const float3 position = { 0, ground + height, z - size.y / 2 };
        const entity character = spawn_character3_player(world, prefab_character3, e, realm, terrain,
            zox_getv(realm, Seed), position, quaternion_identity, "zeland_player");
        zox_remove(character, DisableMovement);
        zox_add(character, PlayerCharacter);
        zox_setv(character, Euler, float3_zero);
        zox_setv(character, FlyMode, 0);
        spawn_capsule_collider(world, character, radius, height, float3_zero);
        zeland_spawn_capsule_mesh(world, character, radius, height);
        zox_setv(e, PlayerStateTarget, zox_player_state_play_trigger);
    }
}

void zox_events_zeland(ecs* world) {
    zox_on_add(game_state_zeland, [in] games.GameState, [none] games.Game, [none] core.Dirty);
    zox_on_add(player_state_zeland, [in] players.PlayerState, [none] players.Player, [none] core.Dirty);
}
