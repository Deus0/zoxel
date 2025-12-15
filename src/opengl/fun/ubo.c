int zox_get_safe_ubo_size() {
    int size = 0;
#ifndef zox_disable_ubos
    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &size);
    // mat4 = 16 floats = 64 bytes
    size = size / (sizeof(float) * 16);
    if (size >= 10000) {
        size = 10000;
    }
#endif
    return size;
}
