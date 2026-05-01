static inline void opengl_bind_texture(uint texture_buffer) {
    glBindTexture(GL_TEXTURE_2D, texture_buffer);
}

static inline void zox_gpu_blend_enable() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

static inline void zox_gpu_blend_disable() {
    glDisable(GL_BLEND);
}

static inline void opengl_disable_texture(byte blend) {
    if (blend) {
        glDisable(GL_BLEND);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

static inline void opengl_clear(const float3 clear_color) {
    if (!headless) {
        glClearColor(clear_color.x, clear_color.y, clear_color.z, 1.0f);
        zox_gpu_clear_viewport();
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }
}

static inline void opengl_clear_viewport_depth() {
    glClear(GL_DEPTH_BUFFER_BIT);
}

static inline void clear_depth_buffer(uint fbo) {
    zox_gpu_bind_fbo(fbo);
    glClear(GL_DEPTH_BUFFER_BIT);
    zox_gpu_bind_fbo(0); // Bind the default framebuffer
}

static inline uint spawn_gpu_texture_buffer() {
    const int texture_type = GL_NEAREST; // GL_NEAREST | GL_LINEAR
    uint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture_type);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture_type);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureID;
}

//! Spawns the buffers for a mesh indicies and verts on the gpu.
uint2 spawn_gpu_mesh_buffers() {
    uint2 mesh = (uint2) { zox_gpu_create_buffer(), zox_gpu_create_buffer() };
    // glGenBuffers(1, &mesh.x);
    // glGenBuffers(1, &mesh.y);
#ifdef zoxel_catch_opengl_errors
    check_opengl_error("spawn_gpu_mesh_buffers");
#endif
    return mesh;
}

static inline uint spawn_gpu_generic_buffer() {
    uint buffer = zox_gpu_create_buffer();
    // glGenBuffers(1, &buffer);
    return buffer;
}

static inline void opengl_bind_mesh(uint2 mesh) {
    zox_gpu_bind_buffer_element(mesh.x);    // for indices
    zox_gpu_bind_buffer_array(mesh.y);            // for vertex coordinates
}

static inline void opengl_set_mesh_indicies(uint indices_buffer) {
    zox_gpu_bind_buffer_element(indices_buffer);    // for indices
}

static inline void opengl_set_mesh_uvs(uint uv_buffer) {
    zox_gpu_bind_buffer_array(uv_buffer);         // for UV coordinates
}

static inline void opengl_unset_mesh() {
    zox_gpu_bind_buffer_element(0);
    zox_gpu_bind_buffer_array(0);
}

static inline void zox_gpu_material(uint material) {
    glUseProgram(material);
}

static inline void zox_disable_material() {
    glUseProgram(0);
}

void zox_gpu_array_buffer_byte(uint shader_index, uint buffer) {
    zox_gpu_bind_buffer_array(buffer);
    glEnableVertexAttribArray(shader_index);
    glVertexAttribPointer(shader_index, 1, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
    zox_gpu_bind_buffer_array(0);
}


/*
Multiple textures:

    https://learnopengl.com/Getting-started/textures

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    // glActiveTexture(GL_TEXTURE0);
    // glUniform1i(texture, 0);
    // glEnableTexture(texture);
    // glActiveTexture(GL_TEXTURE0);
    // glUniform1i(texture, 0);
    // Error 0x500/1280 means GL_INVALID_ENUM

*/
