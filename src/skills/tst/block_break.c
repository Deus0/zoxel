// Exercise the same damage entry point used by melee. Alternate the two faces of a chunk seam.
static entity break_streamer, break_chunk;
static byte3 break_pos;
static int break_gpu_faces;
static int break_trial, break_stage, break_frames;

// Check the -X face of the right chunk in the buffers used by its current draw.
static int right_face_drawn(ecs* world, entity chunk, byte3 pos, float block_scale) {
    entity meshes[zox_chunk_meshes_max];
    const uint count = get_chunk_meshes(world, chunk, meshes);
    for (uint m = 0; m < count; m++) {
        const entity mesh = meshes[m];
        uint indices = zox_getv(mesh, MeshRenderCount);
        if (!indices || zox_has(mesh, Disabled)) continue;
        const byte gpu = zox_has(mesh, GpuMesh);
        if (gpu && !zox_gpu_storage_read(zox_getv(mesh, MeshCommandGPULink).x, 0, sizeof(indices), &indices)) return -1;
        if (!indices) continue;
        const uint stride = gpu ? 4 : 3;
        const size_t bytes = (indices / 6) * 4 * stride * sizeof(float);
        float* vertices = malloc(bytes);
        if (!vertices || !zox_gpu_storage_read(zox_getv(mesh, MeshGPULink).y, 0, bytes, vertices)) {
            free(vertices);
            return -1;
        }
        const float unit = gpu ? 1 : block_scale;
        const float y = (pos.y + 0.5f) * unit, z = (pos.z + 0.5f) * unit;
        for (uint f = 0; f < indices / 6; f++) {
            const float* v = vertices + f * 4 * stride;
            if (v[0] || v[stride] || v[2*stride] || v[3*stride]) continue;
            const float* opposite = v + 2 * stride;
            if (fminf(v[1], opposite[1]) < y && y < fmaxf(v[1], opposite[1]) &&
                fminf(v[2], opposite[2]) < z && z < fmaxf(v[2], opposite[2])) {
                if (gpu) break_gpu_faces++;
                free(vertices);
                return 1;
            }
        }
        free(vertices);
    }
    return 0;
}

// Count faces emitted by this specific voxel in the displayed solid GPU meshes.
// The packed direction distinguishes its face from the neighbour's opposite face.
static int punched_faces(ecs* world, entity chunk, byte3 pos) {
    entity meshes[zox_chunk_meshes_max];
    const uint count = get_chunk_meshes(world, chunk, meshes);
    int found = 0;
    for (uint m = 0; m < count; m++) {
        const entity mesh = meshes[m];
        if (!zox_has(mesh, GpuMesh) || zox_has(mesh, Disabled) || !zox_getv(mesh, MeshRenderCount)) continue;
        uint indices;
        if (!zox_gpu_storage_read(zox_getv(mesh, MeshCommandGPULink).x, 0, sizeof(indices), &indices)) return -1;
        if (!indices) continue;
        const size_t bytes = indices / 6 * 4 * sizeof(float4);
        float4* vertices = malloc(bytes);
        if (!vertices || !zox_gpu_storage_read(zox_getv(mesh, MeshGPULink).y, 0, bytes, vertices)) {
            free(vertices);
            return -1;
        }
        for (uint f = 0; f < indices / 6; f++) {
            const float4 a = vertices[4*f], b = vertices[4*f+2];
            const int direction = ((int)a.w >> 8) & 7;
            if (direction >= 6) continue;
            const int3 n = get_direction_int3(direction);
            if (floorf((a.x + b.x - n.x)*0.5f) == pos.x &&
                floorf((a.y + b.y - n.y)*0.5f) == pos.y &&
                floorf((a.z + b.z - n.z)*0.5f) == pos.z) found++;
        }
        free(vertices);
    }
    return found;
}

static int visible_breaks, break_clear_frames;
static byte moving_break_test;
static byte awaiting_first_dispatch;
static int first_dispatch_checks;

static void check_first_break_dispatch(ecs* world, entity chunk, entity mesh, byte depth, const byte* grid) {
    if (!awaiting_first_dispatch || chunk != break_chunk || zox_has(mesh, Disabled) ||
        depth != zox_getv(chunk, NodeDepth) || getv_VoxelNode(zox_get(chunk, VoxelNode), depth, break_pos)) return;
    awaiting_first_dispatch = 0;
    first_dispatch_checks++;
    const int length = (1 << depth) + 2;
    const int cell = ((break_pos.x + 1) * length + break_pos.y + 1) * length + break_pos.z + 1;
    const int faces = punched_faces(world, chunk, break_pos);
    zox_tst_check(!grid[cell] && faces == 0,
        "first dispatch after break [%i] uploaded voxel [%i], displayed faces [%i], local [%i %i %i]",
        break_trial, grid[cell], faces, break_pos.x, break_pos.y, break_pos.z);
}
static double break_hit_time;

static void moving_break_setup(void) {
    moving_break_test = 1;
    test_gpu_chunk_dispatched = check_first_break_dispatch;
    zox_chunks3_gpu_meshing = 1;
}
static byte block_break_ready(ecs* world) {
    const entity terrain = zox_tst_world_ready(world);
    if (!terrain) return 0;
    if (!break_streamer) {
        break_streamer = spawn_streamer(world, prefab_streamer, terrain, float3_zero);
        zox_setv(break_streamer, StreamerLevel, 1);
        return 0;
    }
    const byte depth = zox_getv(terrain, NodeDepth);
    const int side = 1 << depth;
    const entity right = int3_hashmap_get(zox_get(terrain, ChunkLinks)->value, (int3){1,-1,0});
    if (!break_stage) {
        if (zox_tst_terrain_busy(world, terrain)) return 0;
        const int x = break_trial % 2;
        break_chunk = int3_hashmap_get(zox_get(terrain, ChunkLinks)->value, (int3){x,-1,0});
        if (!zox_valid(right) || !zox_valid(break_chunk)) return 0;
        const BlockLinks* blocks = zox_get(zox_get_link(world, break_chunk, BlockManagerLink), BlockLinks);
        const int column = break_trial / 2;
        if (!x) {
            break_pos = (byte3){x ? 0 : side - 1, 0, column % side};
            int y = side - 1 - column / side;
            for (; y >= 0; y--) {
                break_pos.y = y;
                const byte id = getv_VoxelNode(zox_get(break_chunk, VoxelNode), depth, break_pos);
                const byte other = getv_VoxelNode(zox_get(right, VoxelNode), depth, (byte3){0,y,break_pos.z});
                if (other && other <= blocks->length && zox_has(blocks->value[other-1], BlockHealth) && !zox_has(blocks->value[other-1], BlockInvinsible) && id && id <= blocks->length && !zox_has(blocks->value[id-1], BlockInvinsible) &&
                    zox_has(blocks->value[id-1], BlockHealth)) break;
            }
            zox_tst_check(y >= 0, "no breakable block for trial [%i]", break_trial);
            if (y < 0) return 1;
            if (right_face_drawn(world, right, break_pos, zox_getv(terrain, BlockScale)) != 0) {
                zox_tst_check(0, "starting shared face was exposed or unreadable");
                return 1;
            }
        } else {
            break_pos.x = 0;
        }
        if (zox_chunks3_gpu_meshing) {
            const int before = punched_faces(world, break_chunk, break_pos);
            if (before < 0) {
                zox_tst_check(0, "could not read the punched mesh");
                return 1;
            }
            visible_breaks += before > 0;
        }
        damage_terrain_block(world, break_chunk, break_pos, 0.25f);
        break_stage = 1;
        break_frames = 0;
        return 0;
    }
    const VoxelNode* tree = zox_get(break_chunk, VoxelNode);
    if (break_stage == 1) {
        const VoxelNode* leaf = get_VoxelNode(tree, depth, break_pos);
        const entity health = leaf && is_linked_VoxelNode(leaf) ? get_entity_VoxelNode(leaf) : 0;
        if (!zox_valid(health) || !zox_has(health, StatValue)) {
            if (++break_frames < 30) return 0;
            zox_tst_check(0, "first hit failed to create block health on trial [%i]", break_trial);
            return 1;
        }
        awaiting_first_dispatch = moving_break_test;
        break_hit_time = zox_current_time;
        damage_terrain_block(world, break_chunk, break_pos, zox_getv(health, StatValue));
        if (moving_break_test) {
            const float width = side * zox_getv(terrain, BlockScale);
            zox_setv(break_streamer, Position3D, ((float3){break_trial % 2 ? 0 : width, 0, 0}));
        }
        break_stage = 2;
        break_frames = 0;
        return 0;
    }
    if (getv_VoxelNode(tree, depth, break_pos)) {
        if (++break_frames < 30) return 0;
        zox_tst_check(0, "lethal hit left voxel solid on trial [%i] chunk x [%i] local [%i %i %i]",
            break_trial, break_trial % 2, break_pos.x, break_pos.y, break_pos.z);
        return 1;
    }
    // Leave the renderer running normally through the edit and its one-second crack fade.
    if (moving_break_test && zox_current_time - break_hit_time < 1.25) return 0;
    // DestroyInTime caps each countdown step at 100 ms. Under load a fading overlay can
    // outlive one wall-clock second. Inspect the geometry independently of its lifetime.
    const int expected = !(break_trial % 2);
    const int drawn = right_face_drawn(world, right, break_pos, zox_getv(terrain, BlockScale));
    const int remaining = zox_chunks3_gpu_meshing ? punched_faces(world, break_chunk, break_pos) : 0;
    if (drawn != expected || remaining != 0) {
        break_clear_frames = 0;
        if (++break_frames < 180) return 0;
        zox_log("[tst] punched voxel still owns [%i] displayed faces before any further edit", remaining);
        zox_tst_check(0, "voxel is air but displayed face [%i], expected [%i], trial [%i] local [%i %i %i]",
            drawn, expected, break_trial, break_pos.x, break_pos.y, break_pos.z);
        return 1;
    }
    // Let pending uploads finish before another break can mask a stale rebuild.
    if (zox_tst_terrain_busy(world, terrain) || ++break_clear_frames < 5) return 0;
    if (awaiting_first_dispatch) {
        zox_tst_check(0, "chunk settled without dispatching the edit on break [%i]", break_trial);
        return 1;
    }
    break_clear_frames = 0;
    break_stage = 0;
    zox_tst_progress(++break_trial);
    return break_trial == 64;
}

static void block_break_check(ecs* world) {
    (void) world;
    if (moving_break_test) {
        test_gpu_chunk_dispatched = NULL;
        zox_tst_check(first_dispatch_checks == 64, "checked [%i/64] first GPU dispatches", first_dispatch_checks);
    }
    zox_log("[tst] block breaks [%i], exposed faces observed on GPU-generated meshes [%i]", break_trial, break_gpu_faces);
    if (zox_chunks3_gpu_meshing) {
        zox_log("[tst] blocks with their own GPU faces visible before the hit [%i]", visible_breaks);
        zox_tst_check(visible_breaks >= 32, "too few visible punched blocks [%i]", visible_breaks);
        zox_tst_check(break_gpu_faces > 0, "GPU meshing enabled but no exposed GPU-generated face observed");
    }
    zox_tst_check(break_trial == 64, "removed [%i/64] blocks across the seam", break_trial);
}

// Break the block under the player's crosshair in a live game, and hold the next edit
// until the old faces vanish. Capture the first before/after pair and any stuck frame.
static int play_break_count, play_break_phase, play_break_aim_frames;
static entity play_break_chunk;
static byte3 play_break_pos;
static double play_break_at;

#if defined(zox_opengl) && defined(zox_windows)
static entity probe_camera;
static float3 probe_center;
static float probe_scale;
static const char* probe_writer;
static float probe_previous_depth;
static int probe_x, probe_y;
static entity probe_chunk, probe_mesh;
static byte probe_lod;
static float probe_draw_scale;

static void probe_draw(entity camera, entity chunk, entity mesh, byte lod, float scale) {
    if (camera != probe_camera || play_break_phase != 2 || zox_current_time - play_break_at < 3) return;
    float depth;
    glReadPixels(probe_x, probe_y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
    if (depth != probe_previous_depth) {
        probe_chunk = chunk;
        probe_mesh = mesh;
        probe_lod = lod;
        probe_draw_scale = scale;
        probe_previous_depth = depth;
        probe_writer = "chunk_gpu_render_system";
    }
}
static int probe_surface = -1, probe_checked;

// Sample the player's depth attachment before HUD composition. Project the front
// intersection of the eye-to-centre ray with the selected voxel into window depth.
static void probe_break_depth(entity camera, const char* system, float4x4 vp, float3 eye, float2 origin, float2 size) {
    if (camera != probe_camera || !play_break_phase || play_break_phase == 3) return;
    const float3 d = float3_sub(eye, probe_center);
    const float extent = fmaxf(fabsf(d.x), fmaxf(fabsf(d.y), fabsf(d.z)));
    if (extent <= probe_scale * 0.5f) { probe_surface = -1; return; }
    const float3 front = float3_add(probe_center, float3_scale(d, probe_scale * 0.5f / extent));
    const float4 clip = float4x4_multiply_float4(vp, (float4){front.x,front.y,front.z,1});
    if (clip.w <= 0 || fabsf(clip.x) >= clip.w || fabsf(clip.y) >= clip.w) { probe_surface = -1; return; }
    const float4 mid = float4x4_multiply_float4(vp, (float4){probe_center.x,probe_center.y,probe_center.z,1});
    const float u = 0.5f + 0.5f * clip.x / clip.w, v = 0.5f + 0.5f * clip.y / clip.w;
    float depth = 1;
    probe_x = (int)(origin.x + size.x*u);
    probe_y = (int)(origin.y + size.y*v);
    glReadPixels(probe_x, probe_y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
    if (!system || depth != probe_previous_depth) probe_writer = system;
    probe_previous_depth = depth;
    const float delta = 2*depth - 1 - clip.z/clip.w;
    const float tolerance = fabsf(mid.z/mid.w - clip.z/clip.w) * 0.12f;
    probe_surface = delta < -tolerance ? -1 : delta <= tolerance ? 1 : 0;
}
#endif

static void play_break_setup(void) {
    zox_chunks3_gpu_meshing = 1;
}

static byte play_break_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) return 0;
    const entity player = zox_tst_player_character(world);
    const entity camera = zox_tst_wind_camera(world, zox_get_link(world, player, SkeletonLink));
    if (!zox_valid(camera) || zox_screenshot_request[0]) return 0;
    zox_tst_waiting = play_break_phase ? "the punched block to clear the player view" : "an exposed terrain face in the player view";
    if (play_break_phase == 3) return 1;
    if (!play_break_phase) {
        if (play_break_aim_frames && play_break_aim_frames % 240 == 0) {
            float3 position = zox_getv(player, Position3D);
            position.x += 4;
            position.y += 1;
            zox_setv(player, Position3D, position);
        }
        const float yaw = play_break_aim_frames / 6 * 0.618f;
        const float pitch = 0.55f + 0.08f * ((play_break_aim_frames / 6) % 9);
        const float4 rotation = euler_to_quaternion((float3){pitch, yaw, 0});
        const float3 ahead = float4_rotate_float3(rotation, (float3){0,0,-1});
        zox_setv(camera, LocalRotation3D, euler_to_quaternion((float3){ahead.y < 0 ? pitch : -pitch, yaw, 0}));
        if (++play_break_aim_frames % 6) return 0;
        const RaycastVoxelData* ray = zox_get(player, RaycastVoxelData);
        if (!ray || ray->result != rayhit_terrain || !zox_valid(ray->chunk) ||
            !zox_valid(ray->hit_block) || zox_has(ray->hit_block, BlockInvinsible) ||
            punched_faces(world, ray->chunk, ray->positionl) <= 0) return 0;
        play_break_chunk = ray->chunk;
        play_break_pos = ray->positionl;
#if defined(zox_opengl) && defined(zox_windows)
        probe_camera = camera;
        probe_scale = zox_getv(local_terrain, BlockScale);
        probe_center = local_block_position_to_real_position(play_break_pos, zox_getv(play_break_chunk, ChunkPosition),
            zox_getv(local_terrain, NodeDepth), probe_scale);
        probe_surface = -1;
        test_camera_depth = probe_break_depth;
        test_chunk_draw = probe_draw;
#endif
        if (!play_break_count)
            snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("player_before.ppm"));
        play_break_phase = 1;
        return 0;
    }
    if (play_break_phase == 1) {
#if defined(zox_opengl) && defined(zox_windows)
        if (probe_surface != 1) { play_break_phase = 0; return 0; }
#endif
        damage_terrain_block(world, play_break_chunk, play_break_pos, 0.25f);
        play_break_at = zox_current_time;
        play_break_phase = 4;
        return 0;
    }
    if (play_break_phase == 4) {
        if (zox_current_time - play_break_at < 0.25) return 0;
        damage_terrain_block(world, play_break_chunk, play_break_pos, 1000);
        play_break_at = zox_current_time;
        play_break_phase = 2;
        return 0;
    }
    if (zox_current_time - play_break_at < 1.5) return 0;
    if (!zox_valid(play_break_chunk)) {
        zox_tst_check(0, "target chunk unloaded during the observation");
        return 1;
    }
    const byte voxel = getv_VoxelNode(zox_get(play_break_chunk, VoxelNode), zox_getv(play_break_chunk, NodeDepth), play_break_pos);
    const int faces = punched_faces(world, play_break_chunk, play_break_pos);
    int surface = 0;
#if defined(zox_opengl) && defined(zox_windows)
    surface = probe_surface;
#endif
    if (play_break_phase == 5) {
        if (zox_current_time - play_break_at < 5) return 0;
        zox_log("[tst] original surface after second edit [%i], voxel [%u], faces [%i]", surface, voxel, faces);
        return 1;
    }
    if (voxel || faces || surface == 1) {
        if (zox_current_time - play_break_at < 5) return 0;
#if defined(zox_opengl) && defined(zox_windows)
        zox_log("[tst] writer is target chunk [%i], mesh [%llu], render depth [%u], draw scale [%f], voxel scale [%f], local [%u %u %u]", probe_chunk == play_break_chunk, (unsigned long long)probe_mesh, probe_lod, probe_draw_scale, probe_scale, play_break_pos.x, play_break_pos.y, play_break_pos.z);
        zox_log("[tst] player camera depth at original block surface [%i], depth writer [%s]", surface, probe_writer ? probe_writer : "clear");
#endif
        zox_tst_check(0, "break [%i] still has voxel [%i] and GPU faces [%i] after five seconds", play_break_count, voxel, faces);
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("player_stuck.ppm"));
        const byte depth = zox_getv(play_break_chunk, NodeDepth);
        const VoxelNode* tree = zox_get(play_break_chunk, VoxelNode);
        for (byte x = 0; x < (1 << depth); x++) {
            const byte3 other = {x, play_break_pos.y, play_break_pos.z};
            if (abs((int)x - play_break_pos.x) < 3 || !getv_VoxelNode(tree, depth, other)) continue;
            damage_terrain_block(world, play_break_chunk, other, 1000);
            zox_log("[tst] second edit at [%u %u %u]", other.x, other.y, other.z);
            play_break_phase = 5;
            play_break_at = zox_current_time;
            return 0;
        }
        play_break_phase = 3;
        return 0;
    }
    if (!play_break_count)
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("player_after.ppm"));
#if defined(zox_opengl) && defined(zox_windows)
    probe_checked += surface == 0;
#endif
    zox_tst_progress(++play_break_count);
    play_break_phase = 0;
    return play_break_count == 32;
}

static void play_break_check(ecs* world) {
    (void)world;
#if defined(zox_opengl) && defined(zox_windows)
    test_camera_depth = NULL;
    test_chunk_draw = NULL;
    zox_log("[tst] player depth behind removed surface on [%i/32] breaks", probe_checked);
    zox_tst_check(probe_checked >= 24, "too few unoccluded post-break depth samples");
#endif
    zox_tst_check(play_break_count == 32, "cleared [%i/32] visible player targets", play_break_count);
}
