void zox_gpu_buffer(uint link, int vert_length, byte byte_size, void* ptr) {
    glBindBuffer(GL_ARRAY_BUFFER, link);
    glBufferData(GL_ARRAY_BUFFER, vert_length * byte_size, ptr, GL_STATIC_DRAW);
}