// Gpu meshing of chunk meshes (doc/plans/gpu_meshing.md): a GpuMesh chunk mesh (zox_chunks3_gpu_meshing) is meshed
//  by a compute shader from its chunk's smooth grid (fun/smooth_grid.c), lit, its material's blocks only; the cpu's
//  mesh build and smooth lights skip it
//  - BuildMesh (voxels changed) or BuildMeshColors (lights changed): its grids are filled on worker threads
//      (GridVoxels, GridLights, after the light writers), then MeshDirty: uploaded and dispatched on the main thread, the
//      grids freed; its new faces draw from that frame
//  - its faces are counted on the cpu from its grid before it's dispatched (gpu_mesh_grid_faces), its room grown to fit
//      then: it never draws a mesh that overflowed (the faces that fit, then nothing till it was meshed again bigger)
//  - its count comes back without waiting (MeshFence): then it's MeshBuilt (a lod swap waits for that); over its
//      room (never, the cpu's count is the shader's rule) it grows and meshes again
//  - its borders: a cell is air where a finer neighbour draws air along it, or one unsettled (smooth_grid_borders)
//  - its terrain holds the compute program and the grid upload buffers (ComputeGPULink, GridGPULink), made on its first
//      mesh, on the main thread with the other gpu uploads; no compute (ES 3.0): its meshes go back to the cpu
//  - chunk_gpu_render_system draws them (texture, light, fog), its terrain's draw program in MaterialGPULink
//  - tst terrain_gpu_mesh

uint zox_chunk_gpu_meshes_grown = 0;   // meshes that didn't fit their room, meshed again (tests)
// shown meshes, already lit, whose lights were baked while the lights round them settled (cleared for a finer lod,
//  flooding): drawn dark, then lit (a dark flash as lods got finer); none while they wait (tst lod_light_flash)
uint zox_chunk_gpu_meshes_baked_dark = 0;
#define zox_chunk_gpu_mesh_budget_ms 2.0     // main thread, uploads and dispatches
#define zox_chunk_gpu_grid_budget_ms 3.0     // a worker thread's, filling grids (as chunk sides)
#define zox_chunk_gpu_grid_bytes ((octree_size(smooth_grid_depth_max) + 2) * (octree_size(smooth_grid_depth_max) + 2) * \
    (octree_size(smooth_grid_depth_max) + 2))

// a mesher for one mesh: its terrain's program and grid buffers (ComputeGPULink, GridGPULink), the mesh's outputs
static inline GpuMesher chunk_gpu_mesher(guint program, guint2 grid, guint2 mesh, guint lights, guint uvs, guint2 command, uint room, ulong fence) {
    GpuMesher mesher;
    memset(&mesher, 0, sizeof(GpuMesher));
    memset(mesher.drawn, 255, sizeof(mesher.drawn));
    memset(mesher.hides, 255, sizeof(mesher.hides));
    mesher.program = program;
    mesher.voxels = grid.x;
    mesher.lights = grid.y;
    mesher.indices = mesh.x;
    mesher.vertices = mesh.y;
    mesher.vertex_lights = lights;
    mesher.vertex_uvs = uvs;
    mesher.command = command.x;
    mesher.count = command.y;
    mesher.max_faces = room;
    mesher.fence = fence;
    mesher.edge_faces = 1;
    return mesher;
}

// a tilemap's uvs on the gpu (its TilemapUVsGPULink, written in place), 0 while it generates
static inline guint chunk_gpu_tilemap_uvs(ecs* world, entity tilemap) {
    if (!zox_valid(tilemap) || !zox_has(tilemap, TilemapUVs) || zox_getv(tilemap, GenerateTexture)) {
        return 0;
    }
    if (!zox_has(tilemap, TilemapUVsGPULink)) {
        return 0;
    }
    if (zox_getv(tilemap, TilemapUVsGPULink)) {
        return zox_getv(tilemap, TilemapUVsGPULink);
    }
    const TilemapUVs* uvs = zox_get(tilemap, TilemapUVs);
    if (!uvs->value || !uvs->length) {
        return 0;
    }
    const guint buffer = zox_gpu_storage_buffer(uvs->length * sizeof(float2), uvs->value);
    zox_get_mut(tilemap, TilemapUVsGPULink)->value = buffer;
    return buffer;
}

// a terrain's compute program and grid buffers, made if it has none, with its draw program (MaterialGPULink: made
//  here, before any draw, so the draw only reads), 0 without compute
//  - written in place (its prefab holds them): set as new components on a new terrain, its first program was lost
//    and made again, the first one leaked (tst gpu_restore_game)
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
    zox_get_mut(terrain, ComputeGPULink)->value = program;
    zox_get_mut(terrain, GridGPULink)->value = *grid;
    zox_setv(terrain, MaterialGPULink, gpu_mesh_textured_program());
    return program;
}

// its neighbours' voxels at the depth they draw (chunk_drawn_depth): a face against voxels a coarser lod doesn't
//  show was a hole where lods met (tst stream_seams); the 6 beside it its faces meet as cpu sides do
//  (chunk_side_depth: open while unsettled, smooth_grid_borders)
static inline void smooth_grid_drawn_depths(ecs* world, smooth_grid* grid, const entity* nearby_chunks) {
    grid->use_neighbor_depths = 1;
    for (byte j = 0; j < 27; j++) {
        const entity chunk = nearby_chunks[j];
        if (j == OCTREE_INDEX(0, 0, 0) || !zox_valid(chunk)) {
            grid->voxel_depths[j] = 0;
            continue;
        }
        const byte3 o = { j % 3, (j / 3) % 3, j / 9 };
        const byte beside = (o.x != 1) + (o.y != 1) + (o.z != 1) == 1;
        grid->voxel_depths[j] = beside ? chunk_side_depth(world, chunk) : chunk_drawn_depth(world, chunk);
    }
}

// fills the meshes with tag (BuildMesh or BuildMeshColors): their chunk's voxels and lights at their depth with a voxel
//  of border from the 26 around it (worker threads, reading the octrees as smooth lights does), then MeshDirty
static inline void chunk_gpu_grid_fill(iter* it, entity tag) {
    zox_sys_budget(chunk_gpu_grid, zox_chunk_gpu_grid_budget_ms);
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDepth);
    zox_sys_out(GridVoxels);
    zox_sys_out(GridLights);
    zox_sys_out(GridPresent);
    zox_sys_out(GridLightsPresent);
    zox_sys_out(GridBorderSamples);
    // what hides a mesh's faces, by voxel (its material's, block_material_hides), for borders with finer lods
    entity manager_cached = 0;
    byte material_cached = 255;
    byte materials[256];
    byte hides[256];
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDepth, depth);
        zox_sys_o(GridVoxels, voxels);
        zox_sys_o(GridLights, lights);
        zox_sys_o(GridPresent, present);
        zox_sys_o(GridLightsPresent, lights_present);
        zox_sys_o(GridBorderSamples, border_samples);
        if (zox_sys_budget_spent(chunk_gpu_grid)) {
            break;
        }
        const entity material = zox_get_parent(world, e);
        const entity chunk = zox_valid(material) ? zox_get_parent(world, material) : 0;
        if (!zox_valid(chunk) || depth->value > zox_getv(chunk, NodeDepth) || depth->value > smooth_grid_depth_max) {
            continue;
        }
        entity nearby_chunks[27];
        fetch_nearby_chunks(world, chunk, zox_get(chunk, ChunkNeighbors)->value, nearby_chunks);
        // A hidden replacement and a first mesh sample the same light grid as a relight.
        if (chunk_grid_lights_settling(world, nearby_chunks)) {
            continue;
        }
        zox_sys_budget_start(chunk_gpu_grid);
        const LightOctree* nearby_lights[27];
        const VoxelNode* nearby_voxels[27];
        spinlock* lights_locks[27];
        spinlock* voxels_locks[27];
        fetch_nearby_lights(world, nearby_chunks, nearby_lights, lights_locks);
        fetch_nearby_voxels(world, nearby_chunks, nearby_voxels, voxels_locks);
        if (chunk_grid_lights_settling(world, nearby_chunks)) {
            __atomic_add_fetch(&zox_chunk_gpu_meshes_baked_dark, 1, __ATOMIC_RELAXED);
        }
        smooth_grid grid = { 0 };
        smooth_grid_drawn_depths(world, &grid, nearby_chunks);
        if (smooth_grid_fill(&grid, nearby_lights, lights_locks, nearby_voxels, voxels_locks, depth->value)) {
            const entity manager = zox_get_link(world, chunk, BlockManagerLink);
            const byte block_material = zox_getv(material, BlockMaterial);
            if (manager != manager_cached || block_material != material_cached) {
                if (manager != manager_cached) {
                    get_block_materials(world, manager, materials);
                }
                manager_cached = manager;
                material_cached = block_material;
                hides[0] = 0;
                for (int v = 1; v < 256; v++) {
                    hides[v] = block_material_hides(materials[v], block_material);
                }
            }
            smooth_grid_borders(&grid, nearby_voxels, voxels_locks, depth->value, hides);
            smooth_grid_keep_border(&grid, border_samples, lights_present->value);
            const int cells = gpu_mesh_grid_bytes(depth->value);
            resize_GridVoxels(voxels, cells);
            resize_GridLights(lights, cells);
            if (voxels->value && lights->value) {
                memcpy(voxels->value, grid.voxels, cells);
                memcpy(lights->value, grid.lights, cells);
            }
            present->value = lights_present->value = 0;
            for (byte j = 0; j < 27; j++) {
                present->value |= (uint) grid.voxels_present[j] << j;
                lights_present->value |= (uint) grid.present[j] << j;
            }
            zox_remove_id(e, tag);
            if (tag == zox_id(BuildMesh) && zox_has(e, BuildMeshColors)) {
                zox_remove(e, BuildMeshColors);
            }
            zox_add(e, MeshDirty);
        }
        free(grid.lights);
        zox_sys_budget_end(chunk_gpu_grid);
    }
}

// NOTE: Fills a gpu chunk mesh's grids when its voxels change
void chunk_gpu_grid_system(iter* it) {
    zox_sys_on_begin();
    chunk_gpu_grid_fill(it, zox_id(BuildMesh));
    zox_sys_on_end();
} zoxd_system(chunk_gpu_grid_system);

// NOTE: Fills a gpu chunk mesh's grids again when its lights change
void chunk_gpu_relight_system(iter* it) {
    zox_sys_on_begin();
    chunk_gpu_grid_fill(it, zox_id(BuildMeshColors));
    zox_sys_on_end();
} zoxd_system(chunk_gpu_relight_system);

#ifdef zox_tests
static void (*test_gpu_chunk_dispatched)(ecs*, entity, entity, byte, const byte*);
#if defined(zox_opengl) && defined(zox_windows)
static void (*test_chunk_draw)(entity, entity, entity, byte, float);
#endif
#endif

// NOTE: Meshes a gpu chunk mesh from its grids (MeshDirty: filled), uploads and dispatches them, then frees them;
//  main thread, with the other gpu uploads, within a budget a frame
void chunk_gpu_mesh_system(iter* it) {
    // the gpu let go of (android's background): nothing's made till it's restored (fun/gpu_restore.c)
    if (did_dispose_resources) {
        return;
    }
    zox_sys_on_begin();
    zox_sys_budget(chunk_gpu_mesh, zox_chunk_gpu_mesh_budget_ms);
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDepth);
    zox_sys_in(MeshGPULink);
    zox_sys_in(ColorsGPULink);
    zox_sys_in(UvsGPULink);
    zox_sys_out(GridVoxels);
    zox_sys_out(GridLights);
    zox_sys_in(GridPresent);
    zox_sys_in(GridLightsPresent);
    zox_sys_out(MeshCommandGPULink);
    zox_sys_out(MeshFaceRoom);
    zox_sys_out(MeshFence);
    entity terrain_cached = 0;
    entity tilemap_cached = 0;
    guint tilemap_uvs = 0;
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
        zox_sys_i(UvsGPULink, uvs);
        zox_sys_o(GridVoxels, grid_voxels);
        zox_sys_o(GridLights, grid_lights);
        zox_sys_i(GridPresent, present);
        zox_sys_i(GridLightsPresent, lights_present);
        zox_sys_o(MeshCommandGPULink, command);
        zox_sys_o(MeshFaceRoom, room);
        zox_sys_o(MeshFence, fence);
        if (zox_sys_budget_spent(chunk_gpu_mesh)) {
            break;
        }
        const entity material = zox_get_parent(world, e);
        const entity chunk = zox_valid(material) ? zox_get_parent(world, material) : 0;
        const entity terrain = zox_valid(chunk) ? zox_get_parent(world, chunk) : 0;
        if (!zox_valid(terrain) || grid_voxels->length != (int) gpu_mesh_grid_bytes(depth->value)) {
            continue;
        }
        if (terrain != terrain_cached) {
            terrain_cached = terrain;
            program = chunk_gpu_terrain(world, terrain, &grid_buffers);
        }
        if (!program) {
            // back to the cpu, its BuildMesh builds it there
            zox_remove(e, GpuMesh);
            zox_remove(e, MeshDirty);
            zox_add(e, BuildMesh);
            continue;
        }
        // its buffers are made by initialize_mesh_system and initialize_mesh_colors_system
        if (!mesh->value.x || !mesh->value.y || !lights->value || !uvs->value) {
            continue;
        }
        // its tilemap's uvs, it waits while that generates (as the cpu build does)
        const entity tilemap = zox_get_link(world, chunk, Tilemap);
        if (tilemap != tilemap_cached) {
            tilemap_cached = tilemap;
            tilemap_uvs = chunk_gpu_tilemap_uvs(world, tilemap);
        }
        if (!tilemap_uvs) {
            continue;
        }
        zox_sys_budget_start(chunk_gpu_mesh);
        if (!command->value.x) {
            command->value = (guint2) {
                zox_gpu_storage_buffer(sizeof(zox_gpu_mesh_command_reset), zox_gpu_mesh_command_reset),
                zox_gpu_storage_buffer_read(sizeof(uint), &zox_gpu_mesh_count_reset) };
        }
        GpuMesher mesher = chunk_gpu_mesher(program, grid_buffers, mesh->value, lights->value, uvs->value, command->value, room->value, fence->value);
        gpu_mesher_set_uvs(&mesher, tilemap_uvs);
        const entity manager = zox_get_link(world, chunk, BlockManagerLink);
        if (manager != manager_cached) {
            manager_cached = manager;
            get_block_materials(world, manager, materials);
        }
        update_light_solidity(world, manager, &solidity_manager, solidity);
        gpu_mesher_set_material(&mesher, materials, zox_getv(material, BlockMaterial));
        mesher.edge_faces = 0;
        // room for its faces, counted first (it never overflows: the draws see the whole new mesh the frame it's
        //  dispatched, a grown buffer's new memory with it), at least a voxel's 6 (vulkan can't bind empty buffers)
        const uint faces = gpu_mesh_grid_faces(&mesher, grid_voxels->value, depth->value, present->value);
        if (faces > room->value || room->value < 6) {
            gpu_mesher_grow(&mesher, faces > 6 ? faces : 6);
            room->value = mesher.max_faces;
        }
        const GpuMeshLighting lighting = gpu_mesh_lighting(solidity);
        const byte lit = !zox_no_lights && grid_lights->value;
        gpu_mesh_grid(&mesher, grid_voxels->value, lit ? grid_lights->value : NULL, depth->value, present->value,
            lights_present->value, lit ? &lighting : NULL);
        fence->value = mesher.fence;
#ifdef zox_tests
        if (test_gpu_chunk_dispatched) {
            test_gpu_chunk_dispatched(world, chunk, e, depth->value, grid_voxels->value);
        }
#endif
        resize_GridVoxels(grid_voxels, 0);
        resize_GridLights(grid_lights, 0);
        zox_remove(e, MeshDirty);
        zox_sys_budget_end(chunk_gpu_mesh);
    }
    zox_sys_on_end();
} zoxd_system(chunk_gpu_mesh_system);

// NOTE: Reads a gpu mesh's face count once the gpu's done (never waits): grows and meshes again if it didn't fit,
//  else it's built, its vertex array made the first time (VaoGPULink: before any draw, so the draw only reads)
void chunk_gpu_mesh_count_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(MeshGPULink);
    zox_sys_in(ColorsGPULink);
    zox_sys_in(UvsGPULink);
    zox_sys_in(MeshCommandGPULink);
    zox_sys_out(MeshFaceRoom);
    zox_sys_out(MeshFence);
    zox_sys_out(MeshRenderCount);
    zox_sys_out(VaoGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(MeshGPULink, mesh);
        zox_sys_i(ColorsGPULink, lights);
        zox_sys_i(UvsGPULink, uvs);
        zox_sys_i(MeshCommandGPULink, command);
        zox_sys_o(MeshFaceRoom, room);
        zox_sys_o(MeshFence, fence);
        zox_sys_o(MeshRenderCount, count);
        zox_sys_o(VaoGPULink, vao);
        if (!fence->value) {
            continue;
        }
        GpuMesher mesher = chunk_gpu_mesher(0, guint2_zero, mesh->value, lights->value, uvs->value, command->value, room->value, fence->value);
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
        if (!vao->value) {
            vao->value = zox_gpu_storage_mesh_vao(mesh->value.y, mesh->value.x, 0, lights->value, 1, uvs->value, 2);
        }
        // built and lit: its lights are written with its faces
        if (!zox_has(e, MeshBuilt)) {
            zox_add(e, MeshBuilt);
        }
        if (!zox_has(e, MeshColorsBuilt)) {
            zox_add(e, MeshColorsBuilt);
        }
    }
    zox_sys_on_end();
} zoxd_system(chunk_gpu_mesh_count_system);

// NOTE: Draws gpu chunk meshes, textured from their tilemap, straight from their gpu buffers (an indirect draw: the
//  gpu's own index count), its terrain's draw program (MaterialGPULink); only reads, so vulkan records it in parts on
//  its worker threads (zox_render3_system_parallel), gl runs it whole
void chunk_gpu_render_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    camera_filtering_begin();
    zox_sys_begin();
    zox_sys_in(RenderDisabled);
    zox_sys_in(RenderDepth);
    zox_sys_in(TransformMatrix);
    zox_sys_in(MeshCommandGPULink);
    zox_sys_in(MeshRenderCount);
    zox_sys_in(VaoGPULink);
    entity terrain_cached = 0;
    entity tilemap_cached = 0;
    guint program = 0;
    byte terrain_depth = 0;
    float terrain_scale = 1;
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDisabled, disabled);
        zox_sys_i(RenderDepth, depth);
        zox_sys_i(TransformMatrix, matrix);
        zox_sys_i(MeshCommandGPULink, command);
        zox_sys_i(MeshRenderCount, count);
        zox_sys_i(VaoGPULink, vao);
        // built once (its count's read, its vertex array made): a mesh meshed again draws its new faces from then
        if (disabled->value || !count->value || !vao->value) {
            continue;
        }
        camera_filtering_check();
        const entity material = zox_get_parent(world, e);
        const entity chunk = zox_valid(material) ? zox_get_parent(world, material) : 0;
        const entity terrain = zox_valid(chunk) ? zox_get_parent(world, chunk) : 0;
        if (!zox_valid(terrain)) {
            continue;
        }
        if (terrain != terrain_cached) {
            terrain_cached = terrain;
            program = zox_has(terrain, MaterialGPULink) ? zox_getv(terrain, MaterialGPULink) : 0;
            terrain_depth = zox_getv(terrain, NodeDepth);
            terrain_scale = zox_getv(terrain, BlockScale);
            if (program) {
                zox_gpu_material(program);
                zox_gpu_program_float4(program, "fog_data", get_fog_value());
                zox_gpu_program_float4(program, "shading", (float4) { 1, 0, 0, 0 });
            }
        }
        if (!program) {
            continue;
        }
        const entity tilemap = zox_get_link(world, e, MaterialLink);
        if (tilemap != tilemap_cached) {
            tilemap_cached = tilemap;
            const guint texture = zox_valid(tilemap) && zox_has(tilemap, TextureGPULink) ? zox_getv(tilemap, TextureGPULink) : 0;
            zox_gpu_bind_texture(texture);
        }
        const float scale = get_chunk_scale(depth->value, terrain_depth, terrain_scale);
        zox_gpu_program_float4x4(program, "mvp_matrix", float4x4_multiply(matrix->value, render_camera_matrix));
        zox_gpu_program_float4(program, "transform", (float4) { 0, 0, 0, scale });
        zox_gpu_draw_indirect(vao->value, command->value.x);
#if defined(zox_tests) && defined(zox_opengl) && defined(zox_windows)
        if (test_chunk_draw) {
            test_chunk_draw(renderer_camera, chunk, e, depth->value, scale);
        }
#endif
        __atomic_add_fetch(&zox_drawn_chunks, 1, __ATOMIC_RELAXED);
    }
    if (program) {
        zox_gpu_reset_texture();
        zox_disable_material();
    }
    zox_sys_on_end();
} zoxd_system(chunk_gpu_render_system);
