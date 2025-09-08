void zox_gpu_array_buffer(
    uint link,
    int length,
    byte byte_size,
    void* ptr
) {
    glBindBuffer(GL_ARRAY_BUFFER, link);
    glBufferData(GL_ARRAY_BUFFER, length * byte_size, ptr, GL_STATIC_DRAW);
}

void zox_gpu_element_buffer(
    uint link,
    int length,
    byte byte_size,
    void* ptr
) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, link);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, length * byte_size, ptr, GL_STATIC_DRAW);
}