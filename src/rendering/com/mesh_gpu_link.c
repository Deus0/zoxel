// extern guint2 spawn_gpu_mesh_buffers();

zoxc_guint2(MeshGPULink); // x is for indicies and verts

ECS_DTOR(MeshGPULink, ptr, {
    zox_gpu_dispose_buffer(ptr->value.x);
    zox_gpu_dispose_buffer(ptr->value.y);
})

void on_destroyed_MeshGPULink(iter *it) {
    byte dbg_log = 0;
    zox_sys_begin();
    zox_sys_out(MeshGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(MeshGPULink, component);
        if (!component->value.x && !component->value.y) {
            continue;
        }
        if (dbg_log) {
            zox_log("MeshGPULink Destroy [%ix%i]", component->value.x, component->value.y);
        }
        zox_gpu_dispose_buffer(component->value.x);
        zox_gpu_dispose_buffer(component->value.y);
        component->value.x = 0;
        component->value.y = 0;
    }
}
