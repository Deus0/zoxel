// Gpu meshing of chunk meshes (doc/plans/gpu_meshing.md): a GpuMesh chunk mesh (zox_chunks3_gpu_meshing) is meshed
//  by a compute shader from its chunk's smooth grid (fun/smooth_grid.c), lit, its material's blocks only; the cpu's
//  mesh build and smooth lights skip it
//  - BuildMesh (voxels changed) or BuildMeshColors (lights changed): its grid's filled and it's dispatched, the tag
//      removed; its new faces draw from that frame
//  - its room starts as a guess (a face every two voxel columns) and stays its size; its count comes back without waiting
//      (MeshFence): over the room it grows and meshes again, else it's MeshBuilt (a lod swap waits for that)
//  - its terrain holds the compute program and the grid upload buffers (ComputeGPULink, GridGPULink), made on its first
//      mesh, on the main thread with the other gpu uploads; no compute (ES 3.0): its meshes go back to the cpu
//  - tst terrain_gpu_mesh

uint zox_chunk_gpu_meshes_grown = 0;   // meshes that didn't fit their room, meshed again (tests)
#define zox_chunk_gpu_mesh_budget_ms 2.0
#define zox_chunk_gpu_grid_bytes ((octree_size(smooth_grid_depth_max) + 2) * (octree_size(smooth_grid_depth_max) + 2) * \
    (octree_size(smooth_grid_depth_max) + 2))

// a mesher for one mesh: its terrain's program and grid buffers (ComputeGPULink, GridGPULink), the mesh's outputs
static inline GpuMesher chunk_gpu_mesher(guint program, guint2 grid, guint2 mesh, guint lights, guint command, uint room, ulong fence) {
    GpuMesher mesher;
    memset(&mesher, 0, sizeof(GpuMesher));
    memset(mesher.drawn, 255, sizeof(mesher.drawn));
    memset(mesher.hides, 255, sizeof(mesher.hides));
    mesher.program = program;
    mesher.voxels = grid.x;
    mesher.lights = grid.y;
    mesher.voxels_size = mesher.lights_size = zox_chunk_gpu_grid_bytes;
    mesher.indices = mesh.x;
    mesher.vertices = mesh.y;
    mesher.vertex_lights = lights;
    mesher.command = command;
    mesher.max_faces = room;
    mesher.fence = fence;
    mesher.edge_faces = 1;
    return mesher;
}

// a terrain's program and grid buffers, made if it has none (set on it), 0 without compute
static inline guint chunk_gpu_terrain(ecs* world, entity terrain, guint2* grid) {
    guint program = zox_has(terrain, ComputeGPULink) ? zox_getv(terrain, ComputeGPULink) : 0;
    if (program) {
        *grid = zox_getv(terrain, GridGPULink);
        return program;
    }
    if (!zox_gpu_compute_supported() || !(program = zox_gpu_compute_program(zox_gpu_mesh_source))) {
        zox_logw("[gpu meshing] no compute shaders (OpenGL ES 3.1+ or OpenGL 4.3+), chunks mesh on the cpu");
        return 0;
    }
    *grid = (guint2) { zox_gpu_storage_buffer(zox_chunk_gpu_grid_bytes, NULL), zox_gpu_storage_buffer(zox_chunk_gpu_grid_bytes, NULL) };
    zox_setv(terrain, ComputeGPULink, program);
    zox_setv(terrain, GridGPULink, *grid);
    return program;
}

// dispatches the meshes with tag (BuildMesh or BuildMeshColors), within a budget a frame
static inline void chunk_gpu_mesh_dispatch(iter* it, entity tag) {
    zox_sys_budget(chunk_gpu_mesh, zox_chunk_gpu_mesh_budget_ms);
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDepth);
    zox_sys_in(MeshGPULink);
    zox_sys_in(ColorsGPULink);
    zox_sys_out(MeshCommandGPULink);
    zox_sys_out(MeshFaceRoom);
    zox_sys_out(MeshFence);
    entity terrain_cached = 0;
    guint program = 0;
    guint2 grid_buffers = guint2_zero;
    entity manager_cached = 0;
    entity solidity_manager = 0;
    byte materials[256];
    byte solidity[zox_light_solidity_length];
    memset(solidity, 1, zox_light_solidity_length);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDepth, depth);
        zox_sys_i(MeshGPULink, mesh);
        zox_sys_i(ColorsGPULink, lights);
        zox_sys_o(MeshCommandGPULink, command);
        zox_sys_o(MeshFaceRoom, room);
        zox_sys_o(MeshFence, fence);
        if (zox_sys_budget_spent(chunk_gpu_mesh)) {
            break;
        }
        const entity material = zox_get_parent(world, e);
        const entity chunk = zox_valid(material) ? zox_get_parent(world, material) : 0;
        const entity terrain = zox_valid(chunk) ? zox_get_parent(world, chunk) : 0;
        if (!zox_valid(terrain) || depth->value > zox_getv(chunk, NodeDepth) || depth->value > smooth_grid_depth_max) {
            continue;
        }
        if (terrain != terrain_cached) {
            terrain_cached = terrain;
            program = chunk_gpu_terrain(world, terrain, &grid_buffers);
        }
        if (!program) {
            // back to the cpu, its BuildMesh builds it there
            zox_remove(e, GpuMesh);
            zox_add(e, BuildMesh);
            continue;
        }
        // its buffers are made by initialize_mesh_system and initialize_mesh_colors_system
        if (!mesh->value.x || !mesh->value.y || !lights->value) {
            continue;
        }
        zox_sys_budget_start(chunk_gpu_mesh);
        if (!command->value) {
            command->value = zox_gpu_storage_buffer(sizeof(zox_gpu_mesh_command_reset), zox_gpu_mesh_command_reset);
        }
        GpuMesher mesher = chunk_gpu_mesher(program, grid_buffers, mesh->value, lights->value, command->value, room->value, fence->value);
        if (!room->value) {
            const uint length = octree_size(depth->value);
            gpu_mesher_grow(&mesher, length * length / 2);
            room->value = mesher.max_faces;
        }
        const entity manager = zox_get_link(world, chunk, BlockManagerLink);
        if (manager != manager_cached) {
            manager_cached = manager;
            get_block_materials(world, manager, materials);
        }
        update_light_solidity(world, manager, &solidity_manager, solidity);
        gpu_mesher_set_material(&mesher, materials, zox_getv(material, BlockMaterial));
        mesher.edge_faces = 0;
        // its chunk's voxels and lights with a voxel of border from the 26 around it
        entity nearby_chunks[27];
        const LightNode* nearby_lights[27];
        const VoxelNode* nearby_voxels[27];
        spinlock* lights_locks[27];
        spinlock* voxels_locks[27];
        fetch_nearby_chunks(world, chunk, zox_get(chunk, ChunkNeighbors)->value, nearby_chunks);
        fetch_nearby_lights(world, nearby_chunks, nearby_lights, lights_locks);
        fetch_nearby_voxels(world, nearby_chunks, nearby_voxels, voxels_locks);
        smooth_grid grid = { 0 };
        if (smooth_grid_fill(&grid, nearby_lights, lights_locks, nearby_voxels, voxels_locks, depth->value)) {
            const GpuMeshLighting lighting = gpu_mesh_lighting(solidity);
            gpu_mesh_smooth_grid(&mesher, &grid, depth->value, zox_no_lights ? NULL : &lighting);
            fence->value = mesher.fence;
            zox_remove_id(e, tag);
            if (tag == zox_id(BuildMesh) && zox_has(e, BuildMeshColors)) {
                zox_remove(e, BuildMeshColors);
            }
        }
        free(grid.lights);
        zox_sys_budget_end(chunk_gpu_mesh);
    }
}

// NOTE: Meshes a chunk mesh on the gpu when its voxels change
void chunk_gpu_mesh_system(iter* it) {
    zox_sys_on_begin();
    chunk_gpu_mesh_dispatch(it, zox_id(BuildMesh));
    zox_sys_on_end();
} zoxd_system(chunk_gpu_mesh_system);

// NOTE: Meshes a chunk mesh on the gpu again when its lights change
void chunk_gpu_relight_system(iter* it) {
    zox_sys_on_begin();
    chunk_gpu_mesh_dispatch(it, zox_id(BuildMeshColors));
    zox_sys_on_end();
} zoxd_system(chunk_gpu_relight_system);

// NOTE: Reads a gpu mesh's face count once the gpu's done (never waits): grows and meshes again if it didn't fit,
//  else it's built
void chunk_gpu_mesh_count_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(MeshGPULink);
    zox_sys_in(ColorsGPULink);
    zox_sys_in(MeshCommandGPULink);
    zox_sys_out(MeshFaceRoom);
    zox_sys_out(MeshFence);
    zox_sys_out(MeshRenderCount);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(MeshGPULink, mesh);
        zox_sys_i(ColorsGPULink, lights);
        zox_sys_i(MeshCommandGPULink, command);
        zox_sys_o(MeshFaceRoom, room);
        zox_sys_o(MeshFence, fence);
        zox_sys_o(MeshRenderCount, count);
        if (!fence->value) {
            continue;
        }
        GpuMesher mesher = chunk_gpu_mesher(0, guint2_zero, mesh->value, lights->value, command->value, room->value, fence->value);
        uint faces = 0;
        if (!gpu_mesher_counted(&mesher, &faces)) {
            continue;
        }
        fence->value = 0;
        if (gpu_mesher_grow(&mesher, faces)) {
            room->value = mesher.max_faces;
            count->value = 0;
            zox_chunk_gpu_meshes_grown++;
            zox_add(e, BuildMesh);
            continue;
        }
        count->value = faces * 6;
        if (!zox_has(e, MeshBuilt)) {
            zox_add(e, MeshBuilt);
        }
    }
    zox_sys_on_end();
} zoxd_system(chunk_gpu_mesh_count_system);
