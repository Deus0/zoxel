// tst: stream_gpu_sides, walking about leaves no holes where lods meet with gpu meshing (lights3/sys/gpu_mesh.c)
//  - gpu meshing on, a world of its own (no ui, npcs or block voxes), a streamer stepping a chunk at a time along x
//    then z, so chunks change lod as it goes
//  - once each step settles: every shown gpu chunk mesh's faces (read back) against a reference's (terrain_gpu_mesh's,
//    the shader's rule on the cpu) from a grid with each neighbour at the depth it draws (smooth_grid_drawn_depths):
//    one missing is a hole where lods meet; one held is hidden behind a neighbour, counted apart
//  - was: the border filled at the mesh's own depth, a coarser neighbour's detail hid faces it didn't draw
//  - run: bash bsh/tst.sh stream_gpu_sides (medium terrain, tst.sh)
#define zox_tst_sgs_steps 8
byte zox_tst_sgs_stage = 0;
int zox_tst_sgs_step = 0;
int zox_tst_sgs_settled = 0;
int zox_tst_sgs_frames = 0;
entity zox_tst_sgs_streamer = 0;
int zox_tst_sgs_meshes = 0, zox_tst_sgs_lods = 0, zox_tst_sgs_wrong = 0, zox_tst_sgs_missing = 0, zox_tst_sgs_extra = 0;

void zox_tst_stream_gpu_sides_setup() {
    zox_chunks3_gpu_meshing = 1;
}

// gpu chunk meshes still building (hidden ones, BuildDisabled, wait to be shown)
static inline int zox_tst_sgs_busy(ecs* world) {
    int busy = 0;
    zox_each_begin(MeshFence);
        zox_each_o(MeshFence, fence);
        busy += zox_has(e, GpuMesh) && !zox_has(e, BuildDisabled) && (fence->value || !zox_has(e, MeshBuilt) || zox_has(e, BuildMesh) ||
            zox_has(e, BuildMeshColors) || zox_has(e, MeshDirty));
    zox_each_end();
    return busy;
}

static void zox_tst_sgs_check(ecs* world) {
    int wrong = 0;
    byte materials[256];
    entity manager_cached = 0;
    zox_each_begin(MeshFence);
        if (!zox_has(e, GpuMesh) || zox_has(e, BuildDisabled)) {
            continue;   // hidden: its build waits till it's shown
        }
        const entity material = zox_get_parent(world, e);
        const entity chunk = zox_valid(material) ? zox_get_parent(world, material) : 0;
        if (!zox_valid(chunk) || !zox_has(chunk, ChunkNeighbors)) {
            continue;
        }
        const byte depth = zox_getv(e, RenderDepth);
        uint faces = 0;
        zox_gpu_storage_read(zox_getv(e, MeshCommandGPULink).y, 0, sizeof(uint), &faces);
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
        smooth_grid_drawn_depths(world, &grid, nearby_chunks);
        for (byte j = 0; j < 27; j++) {
            zox_tst_sgs_lods += grid.voxel_depths[j] && grid.voxel_depths[j] < depth;
        }
        if (smooth_grid_fill(&grid, nearby_lights, lights_locks, nearby_voxels, voxels_locks, depth)) {
            zox_tst_sgs_meshes++;
            const uint reference = zox_tst_gpu_terrain_reference(&grid, depth, materials, zox_getv(material, BlockMaterial));
            if (reference != faces) {
                zox_tst_sgs_missing += reference > faces ? reference - faces : 0;
                zox_tst_sgs_extra += faces > reference ? faces - reference : 0;
                if (wrong++ < 6) {
                    const int3 p = zox_getv(chunk, ChunkPosition);
                    zox_log("[tst]  chunk [%ix%ix%i] mesh depth [%i]: [%u] faces, as its neighbours draw [%u]",
                        p.x, p.y, p.z, depth, faces, reference);
                }
            }
        }
        free(grid.lights);
    zox_each_end();
    zox_tst_sgs_wrong += wrong;
    zox_log("[tst] step [%i]: [%i] gpu meshes against what their neighbours draw", zox_tst_sgs_step, wrong);
}

byte zox_tst_stream_gpu_sides_ready(ecs* world) {
    const entity terrain = zox_tst_world_ready(world);
    if (!terrain) {
        return 0;
    }
    const float chunk = octree_size(zox_getv(terrain, NodeDepth)) * zox_getv(terrain, BlockScale);
    zox_tst_progress(zox_tst_sgs_step * 10000 + zox_tst_sgs_frames);
    zox_tst_sgs_frames++;
    if (zox_tst_sgs_stage == 0) {
        zox_tst_sgs_streamer = spawn_streamer(world, prefab_streamer, terrain, (float3) { chunk * 0.5f, chunk, chunk * 0.5f });
        zox_setv(zox_tst_sgs_streamer, StreamerLevel, 1);
        zox_tst_sgs_stage = 1;
        return 0;
    }
    zox_tst_waiting = "the stream and its gpu meshes to settle";
    const byte terrain_busy = zox_tst_terrain_busy(world, terrain);
    const int meshes_busy = zox_tst_sgs_busy(world);
    zox_tst_sgs_settled = terrain_busy || meshes_busy ? 0 : zox_tst_sgs_settled + 1;
    if (zox_tst_sgs_frames < 10 || zox_tst_sgs_settled < 3) {
        return 0;
    }
    zox_tst_sgs_check(world);
    if (zox_tst_sgs_step >= zox_tst_sgs_steps) {
        return 1;
    }
    float3 position = zox_getv(zox_tst_sgs_streamer, Position3D);
    if (zox_tst_sgs_step < zox_tst_sgs_steps / 2) {
        position.x += chunk;
    } else {
        position.z += chunk;
    }
    zox_setv(zox_tst_sgs_streamer, Position3D, position);
    zox_tst_sgs_step++;
    zox_tst_sgs_settled = 0;
    zox_tst_sgs_frames = 0;
    return 0;
}

void zox_tst_stream_gpu_sides_check(ecs* world) {
    (void) world;
    zox_log("[tst] [metric] stream_gpu_sides [%i] gpu mesh checks, [%i] sides beside a coarser lod, [%i] wrong: "
        "faces missing [%i], hidden ones held [%i]",
        zox_tst_sgs_meshes, zox_tst_sgs_lods, zox_tst_sgs_wrong, zox_tst_sgs_missing, zox_tst_sgs_extra);
    zox_tst_check(zox_tst_sgs_step >= zox_tst_sgs_steps, "the stream stopped at step [%i]", zox_tst_sgs_step);
    zox_tst_check(zox_tst_sgs_meshes, "no gpu chunk meshes were checked");
    zox_tst_check(zox_tst_sgs_lods, "no mesh was beside a coarser lod, nothing was checked");
    zox_tst_check(!zox_tst_sgs_missing, "[%i] faces missing where lods meet (holes)", zox_tst_sgs_missing);
    // hidden faces held: behind a neighbour's voxels, not seen (as stream_sides; 215 off in 112 meshes before the fix, 5 after)
    zox_tst_check(zox_tst_sgs_extra < 30, "[%i] faces held that a neighbour hides", zox_tst_sgs_extra);
}

void zox_tsts_stream_gpu_sides(ecs* world) {
    (void) world;
    zox_tst_add("stream_gpu_sides", zox_tst_stream_gpu_sides_ready, zox_tst_stream_gpu_sides_check, 180);
    zox_tst_set_setup("stream_gpu_sides", zox_tst_stream_gpu_sides_setup, 1);
}
