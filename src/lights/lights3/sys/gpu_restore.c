// Gpu meshing's gpu dispose and restore (rendering/fun/resource_management.c): an app minimized, android's lost surface
//  - the generic walks (rendering/fun/gpu_links.c) let go of a gpu mesh's buffers and give it plain empty ones back,
//    and queue MeshDirty; here the rest: each mesh's command, fence and vertex array, its terrain's compute program and
//    grid buffers, the tilemaps' uvs (TilemapUVsGPULink)
//  - restored, all made again in the same frame: each terrain's program, grids and draw program (chunk_gpu_terrain),
//    the tilemaps' uvs (chunk_gpu_tilemap_uvs), each mesh's command; each gpu mesh meshes again from its grids
//    (BuildMesh, not MeshDirty: its grids were freed once dispatched), its room 0 so its buffers grow again
//  - disposed, chunk_gpu_mesh_system makes nothing (did_dispose_resources)
//  - the terrain came back without its chunks after a vulkan restore on android (play test 2026-11-21)
entity_array_d* disposed_gpu_meshes = NULL;
entity_array_d* disposed_gpu_terrains = NULL;
entity_array_d* disposed_gpu_tilemaps = NULL;

void gpu_dispose_gpu_meshing(ecs* world) {
    zox_each_begin(MeshCommandGPULink);
        zox_each_o(MeshCommandGPULink, command);
        if (command->value.x) {
            zox_gpu_dispose_buffer(command->value.x);
            zox_gpu_dispose_buffer(command->value.y);
            command->value = guint2_zero;
        }
        zox_disposed_add(&disposed_gpu_meshes, e);
    zox_each_end();
    zox_each_begin(MeshFence);
        zox_each_o(MeshFence, fence);
        if (fence->value) {
            zox_gpu_fence_dispose(fence->value);
            fence->value = 0;
        }
    zox_each_end();
    zox_each_begin(VaoGPULink);
        zox_each_o(VaoGPULink, vao);
        if (vao->value) {
            zox_gpu_dispose_vao(vao->value);
            vao->value = 0;
        }
    zox_each_end();
    zox_each_begin(ComputeGPULink);
        zox_each_o(ComputeGPULink, program);
        if (!program->value) continue;
        zox_gpu_dispose_program(program->value);
        program->value = 0;
        zox_disposed_add(&disposed_gpu_terrains, e);
    zox_each_end();
    zox_each_begin(TilemapUVsGPULink);
        zox_each_o(TilemapUVsGPULink, uvs);
        if (!uvs->value) continue;
        zox_gpu_dispose_buffer(uvs->value);
        uvs->value = 0;
        zox_disposed_add(&disposed_gpu_tilemaps, e);
    zox_each_end();
    zox_each_begin(GridGPULink);
        zox_each_o(GridGPULink, grid);
        if (grid->value.x) {
            zox_gpu_dispose_buffer(grid->value.x);
            zox_gpu_dispose_buffer(grid->value.y);
            grid->value = guint2_zero;
        }
    zox_each_end();
}

void gpu_restore_gpu_meshing(ecs* world) {
    zox_disposed_begin(disposed_gpu_tilemaps, TilemapUVsGPULink, uvs);
        chunk_gpu_tilemap_uvs(world, e);
    zox_disposed_end(disposed_gpu_tilemaps);
    zox_disposed_begin(disposed_gpu_terrains, ComputeGPULink, program);
        guint2 grid;
        chunk_gpu_terrain(world, e, &grid);
    zox_disposed_end(disposed_gpu_terrains);
    zox_disposed_begin(disposed_gpu_meshes, MeshFaceRoom, room);
        const byte meshed = room->value != 0;   // it had a command (made with its first mesh)
        room->value = 0;
        MeshCommandGPULink* command = zox_get_mut(e, MeshCommandGPULink);
        if (meshed && command && !command->value.x) {
            command->value = (guint2) {
                zox_gpu_storage_buffer(sizeof(zox_gpu_mesh_command_reset), zox_gpu_mesh_command_reset),
                zox_gpu_storage_buffer_read(sizeof(uint), &zox_gpu_mesh_count_reset) };
        }
        if (zox_has(e, MeshRenderCount)) {
            zox_setv(e, MeshRenderCount, 0);
        }
        zox_remove(e, MeshDirty);
        zox_add(e, BuildMesh);
    zox_disposed_end(disposed_gpu_meshes);
}
