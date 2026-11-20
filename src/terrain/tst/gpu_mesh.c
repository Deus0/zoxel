// tst: terrain_gpu_mesh, streamed terrain's solid chunk meshes meshed on the gpu (lights3/sys/gpu_mesh.c)
//  - gpu meshing on (its setup, as --gpu_meshing), a world of its own streamed round a streamer at the origin
//  - waits till every gpu mesh is built (counted, it fit) and none is meshing, then for each: no cpu mesh was built
//      for it, its draw count is its faces, and its faces are a reference's from its chunk's smooth grid (the shader's
//      rule on the cpu: its material's voxels, faces its neighbours, maybe in the next chunk, don't hide, into a
//      missing chunk only the top), so the right chunk, depth, material and neighbours were meshed
//  - run: bash bsh/tst.sh terrain_gpu_mesh
entity zox_tst_gpu_terrain_streamer = 0;
double zox_tst_gpu_terrain_settled = 0;
int zox_tst_gpu_terrain_meshes = 0;

void zox_tst_gpu_terrain_setup() {
    zox_chunks3_gpu_meshing = 1;
}

// gpu meshes, and how many are built and not meshing
static inline int zox_tst_gpu_terrain_count(ecs* world, int* settled) {
    int count = 0;
    *settled = 0;
    zox_each_begin(MeshFence);
        zox_each_o(MeshFence, fence);
        if (!zox_has(e, GpuMesh)) {
            continue;
        }
        count++;
        *settled += !fence->value && zox_has(e, MeshBuilt) && !zox_has(e, BuildMesh) && !zox_has(e, BuildMeshColors) &&
            !zox_has(e, MeshDirty);
    zox_each_end();
    return count;
}

byte zox_tst_gpu_terrain_ready(ecs* world) {
    const entity terrain = zox_tst_world_ready(world);
    if (!terrain) {
        return 0;
    }
    if (!zox_tst_gpu_terrain_streamer) {
        zox_tst_gpu_terrain_streamer = spawn_streamer(world, prefab_streamer, terrain, float3_zero);
        zox_setv(zox_tst_gpu_terrain_streamer, StreamerLevel, 1);
        zox_setv(zox_tst_gpu_terrain_streamer, StreamDirty, zox_dirty_trigger);
        zox_setv(zox_tst_gpu_terrain_streamer, StreamDirty2, zox_dirty_trigger);
    }
    zox_tst_waiting = "every gpu chunk mesh built, none meshing, for a second";
    int settled = 0;
    const int count = zox_tst_gpu_terrain_count(world, &settled);
    zox_tst_progress(count * 1000 + settled);
    if (!count || settled != count || count != zox_tst_gpu_terrain_meshes) {
        zox_tst_gpu_terrain_meshes = count;
        zox_tst_gpu_terrain_settled = zox_current_time;
        return 0;
    }
    return zox_current_time - zox_tst_gpu_terrain_settled > 1;
}

// the faces of a material in a chunk's smooth grid, as the shader meshes them
static inline uint zox_tst_gpu_terrain_reference(const smooth_grid* grid, byte depth, const byte* materials, byte material) {
    const int size = octree_size(depth);
    const int length = grid->length;
    uint faces = 0;
    for (int x = 0; x < size; x++) for (int y = 0; y < size; y++) for (int z = 0; z < size; z++) {
        const byte voxel = grid->voxels[((x + 1) * length + y + 1) * length + z + 1];
        if (!voxel || materials[voxel] != material) {
            continue;
        }
        for (byte d = 0; d < 6; d++) {
            const int3 n = int3_add((int3) { x, y, z }, get_direction_int3(d));
            const int sx = n.x < 0 ? -1 : n.x >= size, sy = n.y < 0 ? -1 : n.y >= size, sz = n.z < 0 ? -1 : n.z >= size;
            if ((sx || sy || sz) && !grid->voxels_present[OCTREE_INDEX(sx, sy, sz)]) {
                faces += d == direction_up;
                continue;
            }
            const byte next = grid->voxels[((n.x + 1) * length + n.y + 1) * length + n.z + 1];
            faces += !block_material_hides(materials[next], material);
        }
    }
    return faces;
}

void zox_tst_gpu_terrain_check(ecs* world) {
    int meshes = 0, wrong = 0, cpu_built = 0, bad_count = 0;
    uint faces_total = 0, room_total = 0;
    byte materials[256];
    entity manager_cached = 0;
    zox_each_begin(MeshFence);
        if (!zox_has(e, GpuMesh)) {
            continue;
        }
        const entity material = zox_get_parent(world, e);
        const entity chunk = zox_valid(material) ? zox_get_parent(world, material) : 0;
        if (!zox_valid(chunk)) {
            continue;
        }
        meshes++;
        const byte depth = zox_getv(e, RenderDepth);
        uint faces = 0;
        zox_gpu_storage_read(zox_getv(e, MeshCommandGPULink).y, 0, sizeof(uint), &faces);
        faces_total += faces;
        room_total += zox_getv(e, MeshFaceRoom);
        cpu_built += zox_get(e, MeshVertices)->length != 0;
        bad_count += zox_getv(e, MeshRenderCount) != faces * 6;
        const entity manager = zox_get_link(world, chunk, BlockManagerLink);
        if (manager != manager_cached) {
            manager_cached = manager;
            get_block_materials(world, manager, materials);
        }
        entity nearby_chunks[27];
        const LightNode* nearby_lights[27];
        const VoxelNode* nearby_voxels[27];
        spinlock* lights_locks[27];
        spinlock* voxels_locks[27];
        fetch_nearby_chunks(world, chunk, zox_get(chunk, ChunkNeighbors)->value, nearby_chunks);
        fetch_nearby_lights(world, nearby_chunks, nearby_lights, lights_locks);
        fetch_nearby_voxels(world, nearby_chunks, nearby_voxels, voxels_locks);
        smooth_grid grid = { 0 };
        if (smooth_grid_fill(&grid, nearby_lights, lights_locks, nearby_voxels, voxels_locks, depth)) {
            const uint reference = zox_tst_gpu_terrain_reference(&grid, depth, materials, zox_getv(material, BlockMaterial));
            if (reference != faces) {
                if (!wrong) {
                    zox_tst_check(0, "chunk [%s] mesh depth [%i]: gpu [%u] faces, the reference [%u]",
                        zox_getn(chunk), depth, faces, reference);
                }
                wrong++;
            }
        }
        free(grid.lights);
    zox_each_end();
    zox_tst_check(meshes, "no gpu chunk meshes");
    zox_tst_check(!wrong, "[%i] of [%i] gpu meshes have other faces than their chunk's", wrong, meshes);
    zox_tst_check(!cpu_built, "[%i] gpu meshes were built on the cpu too", cpu_built);
    zox_tst_check(!bad_count, "[%i] gpu meshes' draw counts aren't their faces", bad_count);
    zox_log("[tst] [metric] terrain_gpu_mesh: [%i] gpu chunk meshes, [%u] faces, room for [%u] (%.0f%% used), [%u] grew",
        meshes, faces_total, room_total, room_total ? 100.0 * faces_total / room_total : 0.0, zox_chunk_gpu_meshes_grown);
}

// # terrain_meshing_view

// tst: terrain_meshing_view, a world's terrain from 4 fixed views, no hud, no player: the same shot with cpu meshing
//  and with --gpu_meshing (bash bsh/compare_meshing.sh compares them), tst/terrain_meshing_view.ppm
//  - waits till every chunk mesh is built and lit and none changes for 2 seconds, then the shot
extern byte nohud;
extern char zox_screenshot_request[256];
byte zox_tst_meshing_view_stage = 0;
int zox_tst_meshing_view_meshes = 0;
double zox_tst_meshing_view_settled = 0;

// chunk meshes, and how many are built, lit and not changing
static inline int zox_tst_meshing_view_count(ecs* world, int* settled) {
    int count = 0;
    *settled = 0;
    zox_each_begin(MeshRenderCount);
        if (!zox_has(e, ChunkMesh) || zox_has(e, Disabled)) {
            continue;
        }
        count++;
        const byte gpu_busy = zox_has(e, GpuMesh) && zox_getv(e, MeshFence);
        *settled += zox_has(e, MeshBuilt) && !gpu_busy && !zox_has(e, BuildMesh) && !zox_has(e, MeshDirty) &&
            !zox_has(e, BuildMeshColors) && !zox_has(e, MeshColorsDirty);
    zox_each_end();
    return count;
}

byte zox_tst_meshing_view_ready(ecs* world) {
    const entity terrain = zox_tst_world_ready(world);
    if (!terrain) {
        return 0;
    }
    if (zox_tst_meshing_view_stage == 0) {
        const entity streamer = spawn_streamer(world, prefab_streamer, terrain, float3_zero);
        zox_setv(streamer, StreamerLevel, 1);
        zox_setv(streamer, StreamDirty, zox_dirty_trigger);
        zox_setv(streamer, StreamDirty2, zox_dirty_trigger);
        zox_tst_meshing_view_stage = 1;
        return 0;
    }
    if (zox_tst_meshing_view_stage == 1) {
        zox_tst_waiting = "every chunk mesh built and lit, none changing for 2 seconds";
        int settled = 0;
        const int count = zox_tst_meshing_view_count(world, &settled);
        zox_tst_progress(count * 1000 + settled);
        if (!count || settled != count || count != zox_tst_meshing_view_meshes) {
            zox_tst_meshing_view_meshes = count;
            zox_tst_meshing_view_settled = zox_current_time;
            return 0;
        }
        if (zox_current_time - zox_tst_meshing_view_settled < 2) {
            return 0;
        }
        // 4 views round the origin, looking down at it from a chunk and a half away
        const float span = octree_size(zox_getv(terrain, NodeDepth)) * zox_getv(terrain, BlockScale);
        for (int i = 0; i < 4; i++) {
            const float angle = 0.6f + i * 1.5708f;
            const float3 eye = { cosf(angle) * span * 1.4f, span * (0.9f + 0.3f * i), sinf(angle) * span * 1.4f };
            const float3 target = { 0, span * 0.3f, 0 };
            const entity camera = spawn_viewport_camera(world, main_app, eye, camera_look_at(eye, target), 60, viewport_grid_cell(i, 2, 2));
            zox_setv(camera, Color, ((color) { 96, 120, 160, 255 }));
        }
        nohud = 1;
        const entity game_view = zox_get_link(world, main_cameras[0], RenderTextureLink);
        if (zox_valid(game_view)) {
            zox_setv(game_view, RenderDisabled, 1);
        }
        zox_tst_meshing_view_stage = 2;
        return 0;
    }
    if (zox_tst_meshing_view_stage == 2) {
        zox_tst_waiting = "the views drawn";
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("terrain_meshing_view.ppm"));
        zox_tst_meshing_view_stage = 3;
        return 0;
    }
    zox_tst_waiting = "the screenshot";
    return !zox_screenshot_request[0];
}

void zox_tst_meshing_view_check(ecs* world) {
    int settled = 0;
    const int count = zox_tst_meshing_view_count(world, &settled);
    zox_tst_check(count, "no chunk meshes");
    zox_log("[tst] terrain_meshing_view: [%i] chunk meshes ([%s] meshing), [%u] chunks drawn", count,
        zox_chunks3_gpu_meshing ? "gpu" : "cpu", zox_drawn_chunks);
}

void zox_tsts_terrain_gpu_mesh(ecs* world) {
    (void) world;
    zox_tst_add("terrain_gpu_mesh", zox_tst_gpu_terrain_ready, zox_tst_gpu_terrain_check, 120);
    zox_tst_set_setup("terrain_gpu_mesh", zox_tst_gpu_terrain_setup, 1);
    zox_tst_add("terrain_meshing_view", zox_tst_meshing_view_ready, zox_tst_meshing_view_check, 120);
    zox_tst_set_setup("terrain_meshing_view", NULL, 1);
}
