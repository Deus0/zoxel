// Define the FrameBufferLink component
zoxc(FrameBufferLink, uint);

// Destructor for FrameBufferLink component
ECS_DTOR(FrameBufferLink, ptr, {
    // zox_log(" - destroying frame buffer %i\n", ptr->value) // this should never really be called tho
    zox_gpu_dispose_fbo(ptr->value);
    // if (ptr->value) glDeleteFramebuffers(1, &ptr->value);

    ptr->value = 0;
})

// Function to generate a frame buffer object on gpu
uint gpu_spawn_frame_buffer_object() {
    uint buffer;
    glGenFramebuffers(1, &buffer);
#ifdef zoxel_catch_opengl_errors
    if (check_opengl_error_unlogged()) zox_log(" ! [gpu_spawn_frame_buffer_object] error at glGenFramebuffers\n")
#endif
    return buffer;
}
// Function to spawn and attach a frame buffer object to an entity
uint spawn_frame_buffer_object(ecs *world, entity e) {
    uint buffer = gpu_spawn_frame_buffer_object();
    zox_set(e, FrameBufferLink, { buffer })
    // zox_log(" + spawn_frame_buffer_object [%u]\n", buffer)

    return buffer;
}

// Attach the texture to the FBO
void connect_render_texture_to_fbo(uint fbo, uint texture) {
    // zox_log(" + connecting texture [%i] to fbo [%i]\n", texture, fbo)
    zox_gpu_link_fbo(fbo, texture);
    /*zox_gpu_bind_fbo(fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
#ifdef zoxel_catch_opengl_errors
    if (check_opengl_error_unlogged()) zox_log(" > failed fbo to texture [%i] : [%i]\n", fbo, texture)
#endif
    zox_gpu_bind_fbo(0);*/
}

void set_render_texture_gpu(uint index, int2 size) {
    // make it a render texture update system
    // zox_gpu_bind_texture(index);
    zox_gpu_set_texture_color_rgb(index, size, NULL);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    // zox_gpu_bind_texture(0);
}
