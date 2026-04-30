void zox_gpu_array_buffer(uint link, int length, byte byte_size, void* ptr) {
    zox_gpu_bind_buffer_array(link);
    zox_gpu_set_buffer_array(ptr, length * byte_size);
    // glBufferData(GL_ARRAY_BUFFER, length * byte_size, ptr, GL_STATIC_DRAW);
}

void zox_gpu_element_buffer(uint link, int length, byte byte_size, void* ptr) {
    zox_gpu_bind_buffer_element(link);
    zox_gpu_set_buffer_element(ptr, length * byte_size);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, length * byte_size, ptr, GL_STATIC_DRAW);
}
