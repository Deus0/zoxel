// A mesh meshed by a compute shader (doc/plans/gpu_meshing.md, chunks3/fun/gpu_mesh.c), not built on the cpu
//  - MeshGPULink holds its indices (x) and vertices (y), ColorsGPULink its vertices' face lights, all written by
//      the gpu, sized for MeshFaceRoom faces
//  - MeshCommandGPULink: its indirect draw command, the gpu writes its index count (and the faces it wanted)
//  - MeshFence: after its last mesh, 0 once its count was read (then it grows if it didn't fit, or it's built)
zox_tag(GpuMesh);
zoxc_guint(MeshCommandGPULink);
zoxc_uint(MeshFaceRoom);
zoxc_ulong(MeshFence);

void on_destroyed_MeshCommandGPULink(iter* it) {
    zox_sys_begin();
    zox_sys_out(MeshCommandGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(MeshCommandGPULink, command);
        if (command->value) {
            zox_gpu_dispose_buffer(command->value);
            command->value = 0;
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
