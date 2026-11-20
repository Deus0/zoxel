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
