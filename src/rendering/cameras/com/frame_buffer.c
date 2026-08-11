// Define the FrameBufferLink component
zoxc_guint(FrameBufferLink);

// Destructor for FrameBufferLink component
/*ECS_DTOR(FrameBufferLink, ptr, {
    zox_gpu_dispose_fbo(ptr->value);
    ptr->value = 0;
})*/

void on_destroyed_FrameBufferLink(iter *it) {
    byte dbg_log = 0;
    zox_sys_begin();
    zox_sys_out(FrameBufferLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(FrameBufferLink, component);
        if (!component->value) {
            continue;
        }
        if (dbg_log) {
            zox_log("FrameBufferLink Destroy [%i]", component->value);
        }
        zox_gpu_dispose_fbo(component->value);
        component->value = 0;
    }
}

// Function to spawn and attach a frame buffer object to an entity
uint spawn_frame_buffer_object(ecs *world, entity e) {
    uint buffer = zox_gpu_create_fbo();
    zox_set(e, FrameBufferLink, { buffer });
    return buffer;
}

void set_render_texture_gpu(guint index, int2 size, byte has_alpha) {
    if (has_alpha) {
        zox_gpu_set_texture_color_rgba(index, size, NULL);
    } else {
        zox_gpu_set_texture_color_rgb(index, size, NULL);
    }
}
