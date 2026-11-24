// A mesh meshed by a compute shader (doc/plans/gpu_meshing.md, chunks3/fun/gpu_mesh.c), not built on the cpu
//  - MeshGPULink holds its indices (x) and vertices (y), ColorsGPULink its vertices' face lights, all written by
//      the gpu, sized for MeshFaceRoom faces
//  - MeshCommandGPULink: x its indirect draw command (the gpu writes its index count), y the faces it wanted (its own
//      buffer: reading one the draws use stalls till they're done)
//  - MeshFence: after its last mesh, 0 once its count was read (then it grows if it didn't fit, or it's built)
zox_tag(GpuMesh);
zoxc_guint2(MeshCommandGPULink);
zoxc_uint(MeshFaceRoom);
zoxc_ulong(MeshFence);
// its chunk's grids to mesh (lights3/fun/smooth_grid.c's layout), filled on worker threads, freed once dispatched
zoxc_arrayd(GridVoxels, byte);
zoxc_arrayd(GridLights, byte);
zoxc_arrayd(GridBorderSamples, ushort);
zoxc_uint(GridPresent);         // nearby chunks with voxels, bit OCTREE_INDEX
zoxc_uint(GridLightsPresent);   // nearby chunks with lights

void on_destroyed_MeshCommandGPULink(iter* it) {
    zox_sys_begin();
    zox_sys_out(MeshCommandGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(MeshCommandGPULink, command);
        if (command->value.x) {
            zox_gpu_dispose_buffer(command->value.x);
            zox_gpu_dispose_buffer(command->value.y);
            command->value = guint2_zero;
        }
    }
}

void on_destroyed_MeshFence(iter* it) {
    zox_sys_begin();
    zox_sys_out(MeshFence);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(MeshFence, fence);
        if (fence->value) {
            zox_gpu_fence_dispose(fence->value);
            fence->value = 0;
        }
    }
}

// a terrain's gpu meshing (lights3/sys/gpu_mesh.c): its chunks' compute program, and the grids they're uploaded
//  through (x voxels, y lights), made on its first gpu mesh
zoxc_guint(ComputeGPULink);
zoxc_guint2(GridGPULink);

void on_destroyed_ComputeGPULink(iter* it) {
    zox_sys_begin();
    zox_sys_out(ComputeGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(ComputeGPULink, program);
        if (program->value) {
            zox_gpu_dispose_program(program->value);
            program->value = 0;
        }
    }
}

void on_destroyed_GridGPULink(iter* it) {
    zox_sys_begin();
    zox_sys_out(GridGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(GridGPULink, grid);
        if (grid->value.x) {
            zox_gpu_dispose_buffer(grid->value.x);
            zox_gpu_dispose_buffer(grid->value.y);
            grid->value = guint2_zero;
        }
    }
}

// a tilemap's uvs as a storage buffer for gpu meshing (24 a block), uploaded on its first gpu mesh: its own link, not
//  UvsGPULink (the mesh initialize systems gave that a plain empty buffer, over the uploaded one, leaking it)
zoxc_guint(TilemapUVsGPULink);

void on_destroyed_TilemapUVsGPULink(iter* it) {
    zox_sys_begin();
    zox_sys_out(TilemapUVsGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(TilemapUVsGPULink, uvs);
        if (uvs->value) {
            zox_gpu_dispose_buffer(uvs->value);
            uvs->value = 0;
        }
    }
}

// a gpu mesh's vertex array (its vertices, face lights, uvs and indices), made on its first draw
zoxc_guint(VaoGPULink);

void on_destroyed_VaoGPULink(iter* it) {
    zox_sys_begin();
    zox_sys_out(VaoGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(VaoGPULink, vao);
        if (vao->value) {
            zox_gpu_dispose_vao(vao->value);
            vao->value = 0;
        }
    }
}
