// Uniform buffers (the instance matrices): kept on the cpu, copied into the frame's ring when a draw reads them
//  - gl's binding points (zox_gpu_ubo_initialize_matricies) name which buffer a program's block reads

// the buffer's cpu copy, sized for length matrices
void vulkan_ubo_write(guint id, gsizei length, gsizei capacity, const void* ptr) {
    vk_buffer* buffer = vk_buffer_of(id);
    if (!buffer) {
        return;
    }
    const VkDeviceSize bytes = (VkDeviceSize) (capacity > length ? capacity : length) * sizeof(float4x4);
    if (bytes > buffer->size || !buffer->shadow) {
        buffer->shadow = realloc(buffer->shadow, bytes);
        memset(buffer->shadow, 0, bytes);
        buffer->size = bytes;
    }
    if (ptr) {
        memcpy(buffer->shadow, ptr, (size_t) length * sizeof(float4x4));
    }
    buffer->shadow_dirty = 1;
}

static inline void zox_gpu_ubo_set_matricies(guint id, gsizei length, const void* ptr) {
    vulkan_ubo_write(id, length, length, ptr);
}

static inline void zox_gpu_ubo_stream_matricies(guint id, gsizei length, gsizei capacity, const void* ptr) {
    vulkan_ubo_write(id, length, capacity, ptr);
}

static inline void zox_gpu_ubo_initialize_matricies(guint id, guint binding_point, gsizei length) {
    vulkan_ubo_write(id, 0, length, NULL);
    if (binding_point < vk_slots_max) {
        vk_ubo_points[binding_point] = id;
    }
}

static inline void zox_gpu_ubo_block_binding(guint program_id, guint block_index, guint binding) {
    vk_program* program = vk_program_of(program_id);
    if (program && block_index < (guint) program->blocks_count) {
        program->blocks[block_index].point = binding;
    }
}

static inline void zox_gpu_ubo_reset() { }

// where a uniform buffer is in this frame's ring, copied there if it changed (or the ring moved on)
byte vulkan_ubo_ring(vk_buffer* buffer, VkBuffer* ring, uint* offset) {
    if (!buffer->shadow) {
        return 0;
    }
    if (buffer->shadow_dirty || buffer->ring_epoch != vk_ring_epoch) {
        if (!vulkan_ring_push(buffer->shadow, buffer->size, &buffer->ring, &buffer->ring_offset)) {
            return 0;
        }
        buffer->ring_epoch = vk_ring_epoch;
        buffer->shadow_dirty = 0;
    }
    *ring = buffer->ring;
    *offset = buffer->ring_offset;
    return 1;
}
