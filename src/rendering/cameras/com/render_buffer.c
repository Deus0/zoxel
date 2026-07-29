zoxc_guint(RenderBufferLink);

// Destructor for RenderBufferLink component
ECS_DTOR(RenderBufferLink, ptr, {
    zox_gpu_dispose_rbo(ptr->value);
    ptr->value = 0;
})

void on_destroyed_RenderBufferLink(iter *it) {
    byte dbg_log = 0;
    zox_sys_begin();
    zox_sys_out(RenderBufferLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(RenderBufferLink, component);
        if (!component->value) {
            continue;
        }
        if (dbg_log) {
            zox_log("RenderBufferLink Destroy [%i]", component->value);
        }
        zox_gpu_dispose_rbo(component->value);
        component->value = 0;
    }
}

// Create and attach a renderbuffer for depth and stencil (optional, depending on needs)
guint gpu_spawn_render_buffer(int2 size) {
    guint rbo = zox_gpu_create_rbo();
    zox_gpu_set_rbo_size(rbo, size);
    return rbo;
}

guint spawn_render_buffer(ecs* world, entity e, int2 size) {
    guint buffer = gpu_spawn_render_buffer(size);
    zox_set(e, RenderBufferLink, { buffer });
    return buffer;
}
