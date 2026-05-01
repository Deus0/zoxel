zoxc(RenderBufferLink, uint);

// Destructor for RenderBufferLink component
ECS_DTOR(RenderBufferLink, ptr, {
    zox_gpu_dispose_rbo(ptr->value);
    ptr->value = 0;
})

// GL_DEPTH24_STENCIL8 GL_DEPTH
void set_render_buffer_size(uint rbo, int2 size) {
#ifndef zox_gles2
    zox_gpu_set_rbo_size(rbo, size);
    // glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.x, size.y);
    // glBindRenderbuffer(GL_RENDERBUFFER, 0);
#endif
}

// Create and attach a renderbuffer for depth and stencil (optional, depending on needs)
uint gpu_spawn_render_buffer(int2 size) {
    uint rbo = zox_gpu_create_rbo();
    set_render_buffer_size(rbo, size);
    return rbo;
}

void prefab_add_render_buffer(ecs *world, entity e) {
    zox_prefab_set(e, RenderBufferLink, { 0 });
}

uint spawn_render_buffer(ecs *world, entity e, int2 size) {
    uint buffer = gpu_spawn_render_buffer(size);
    zox_set(e, RenderBufferLink, { buffer });
    return buffer;
}

void connect_render_buffer_to_fbo(uint fbo, uint rbo) {
    zox_gpu_link_fbo_rbo(fbo, rbo);
}
