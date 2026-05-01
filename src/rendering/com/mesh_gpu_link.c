extern uint2 spawn_gpu_mesh_buffers();

zoxc_uint2(MeshGPULink); // x is for indicies and verts

void add_gpu_mesh(ecs *world, entity e) {
    zox_prefab_set(e, MeshGPULink, { { 0, 0 } });
}

void spawn_gpu_mesh(ecs *world, entity e) {
    zox_set(e, MeshGPULink, { spawn_gpu_mesh_buffers() });
}

ECS_DTOR(MeshGPULink, ptr, {
    zox_gpu_dispose_buffer(ptr->value.x);
    zox_gpu_dispose_buffer(ptr->value.y);
})
