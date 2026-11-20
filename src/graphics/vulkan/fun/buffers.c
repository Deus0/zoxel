// Buffers as gl's: an id bound to the array or element target, filled by glBufferData's calls
//  - a buffer a submission still reads gets new memory when it's written (gl's orphaning), the old is freed after

static inline guint zox_gpu_create_buffer() {
    const uint id = vk_table_add(&vk_buffers);
    vk_buffer* buffer = vk_table_get(&vk_buffers, id);
    buffer->live = 1;
    zox_gpu_buffers_live++;
    return id;
}

static inline guint2 spawn_gpu_mesh_buffers() {
    return (guint2) { zox_gpu_create_buffer(), zox_gpu_create_buffer() };
}

void vulkan_buffer_release(vk_buffer* buffer) {
    if (buffer->allocation) {
        vulkan_garbage_add((vk_garbage_item) { .allocation = buffer->allocation, .serial = buffer->used });
    }
    buffer->buffer = VK_NULL_HANDLE;
    buffer->allocation = 0;
    buffer->offset = 0;
    buffer->mapped = NULL;
    buffer->capacity = 0;
}

static inline void zox_gpu_dispose_buffer(guint id) {
    vk_buffer* buffer = vk_buffer_of(id);
    if (!buffer) {
        return;
    }
    vulkan_buffer_release(buffer);
    free(buffer->shadow);
    buffer->live = 0;
    vk_table_remove(&vk_buffers, id);
    zox_gpu_buffers_live--;
    zox_gpu_buffers_freed++;
}

// a copy recorded in the frame's upload commands (submitted before its draws), then a barrier: the next copy or
// draw sees what it wrote
void vulkan_copy_buffer(VkBuffer from, VkDeviceSize from_offset, VkBuffer to, VkDeviceSize to_offset, VkDeviceSize bytes) {
    pthread_mutex_lock(&vk_lock);   // a recording worker can upload (particles)
    const VkCommandBuffer commands = vulkan_upload_commands();
    const VkBufferCopy copy = { from_offset, to_offset, bytes };
    vkCmdCopyBuffer(commands, from, to, 1, &copy);
    const VkMemoryBarrier barrier = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER,
        .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
        .dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT | VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT
            | VK_ACCESS_INDEX_READ_BIT,
    };
    vkCmdPipelineBarrier(commands, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT
        | VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0, 1, &barrier, 0, NULL, 0, NULL);
    pthread_mutex_unlock(&vk_lock);
}

// bytes into the buffer at offset: mapped, a memcpy; gpu only, through a staging slot and a copy
void vulkan_buffer_write(vk_buffer* buffer, VkDeviceSize offset, const void* data, VkDeviceSize bytes) {
    if (buffer->mapped) {
        memcpy(buffer->mapped + offset, data, bytes);
        return;
    }
    const vk_allocation staging = vulkan_slab_allocate(bytes, 0);
    if (!staging.handle) {
        return;
    }
    memcpy(staging.mapped, data, bytes);
    vulkan_copy_buffer(staging.buffer, staging.offset, buffer->buffer, buffer->offset + offset, bytes);
    // the copy is in the next submission: the staging slot freed after it, the buffer read by it
    buffer->used = vk_serial_recording();
    vulkan_garbage_add((vk_garbage_item) { .allocation = staging.handle, .serial = buffer->used });
}

// room for bytes, new memory if it's too small or still read (keep: the old bytes are copied over)
byte vulkan_buffer_reserve(vk_buffer* buffer, VkDeviceSize bytes, byte keep) {
    if (buffer->buffer && bytes <= buffer->capacity && !vk_busy(buffer->used)) {
        return 1;
    }
    // a little over: R8G8B8 colors read as R8G8B8A8 on gpus without it read a byte past the last
    const vk_allocation allocation = vulkan_slab_allocate((bytes > buffer->capacity ? bytes : buffer->capacity) + 16,
        vk_meshes_staged);
    if (!allocation.handle) {
        return 0;
    }
    ulong used = 0;
    if (keep && buffer->size && buffer->buffer) {
        if (buffer->mapped && allocation.mapped) {
            memcpy(allocation.mapped, buffer->mapped, buffer->size);
        } else {
            vulkan_copy_buffer(buffer->buffer, buffer->offset, allocation.buffer, allocation.offset, buffer->size);
            used = vk_serial_recording();
            buffer->used = used;    // the old one's read by the copy
        }
    }
    vulkan_buffer_release(buffer);
    buffer->buffer = allocation.buffer;
    buffer->offset = allocation.offset;
    buffer->allocation = allocation.handle;
    buffer->mapped = allocation.mapped;
    buffer->capacity = allocation.size - 16;
    buffer->used = used;
    return 1;
}

// gl's glBufferData: the buffer is these bytes now
void vulkan_buffer_data(uint id, const void* data, VkDeviceSize bytes) {
    vk_buffer* buffer = vk_buffer_of(id);
    if (!buffer || !vulkan_buffer_reserve(buffer, bytes ? bytes : 4, 0)) {
        return;
    }
    if (data && bytes) {
        vulkan_buffer_write(buffer, 0, data, bytes);
    }
    buffer->size = bytes;
}

// gl's glBufferSubData at the start of the buffer, but a buffer moved (still read) keeps none of the old bytes past
// the write: its callers (particles, instanced lines) draw only what they wrote each frame, and copying the rest
// read back gpu memory every frame (14% of the main thread at far 8)
void vulkan_buffer_sub_data(uint id, const void* data, VkDeviceSize bytes) {
    vk_buffer* buffer = vk_buffer_of(id);
    if (!buffer || !data || !bytes) {
        return;
    }
    if (!vulkan_buffer_reserve(buffer, bytes > buffer->size ? bytes : buffer->size, 0)) {
        return;
    }
    vulkan_buffer_write(buffer, 0, data, bytes);
    if (bytes > buffer->size) {
        buffer->size = bytes;
    }
}

static inline void zox_gpu_bind_buffer_element(guint v) {
    vk_ctx->element_bound = v;
}

static inline void zox_gpu_bind_buffer_array(guint v) {
    vk_ctx->array_bound = v;
}

static inline void zox_gpu_set_buffer_element(const void* ptr, gsize length) {
    vulkan_buffer_data(vk_ctx->element_bound, ptr, length);
}

static inline void zox_gpu_set_buffer_array(const void* ptr, gsize length) {
    vulkan_buffer_data(vk_ctx->array_bound, ptr, length);
}

static inline void zox_gpu_set_sub_buffer_float3(gsize length, const void* ptr) {
    vulkan_buffer_sub_data(vk_ctx->array_bound, ptr, length * sizeof(float3));
}

static inline void zox_gpu_set_sub_buffer_color(gsize length, const void* ptr) {
    vulkan_buffer_sub_data(vk_ctx->array_bound, ptr, length * sizeof(color));
}

static inline void zox_gpu_set_sub_buffer_bytes(gsize bytes, const void* ptr) {
    vulkan_buffer_sub_data(vk_ctx->array_bound, ptr, bytes);
}

static inline void zox_gpu_array_buffer(guint link, gsize length, byte byte_size, void* ptr) {
    vulkan_buffer_data(link, ptr, (VkDeviceSize) length * byte_size);
}

static inline void zox_gpu_element_buffer(guint link, gsize length, byte byte_size, void* ptr) {
    vulkan_buffer_data(link, ptr, (VkDeviceSize) length * byte_size);
}

static inline void zox_gpu_reset_mesh() {
    vk_ctx->element_bound = 0;
    vk_ctx->array_bound = 0;
}

// Attributes: gl's glVertexAttribPointer reads the bound array buffer (or client memory when none is)

// R8G8B8 vertices are optional: read as R8G8B8A8 where they're not (the buffers have room past the end)
static inline VkFormat vulkan_rgb8_format() {
    return vk_rgb8_vertex ? VK_FORMAT_R8G8B8_UNORM : VK_FORMAT_R8G8B8A8_UNORM;
}

void vulkan_attribute(guint location, VkFormat format, uint stride, const void* pointer) {
    if (location >= vk_slots_max) {
        return;
    }
    vk_attribute* attribute = &vk_ctx->attributes[location];
    attribute->buffer = vk_ctx->array_bound;
    attribute->pointer = pointer;
    attribute->format = format;
    attribute->stride = stride;
}

static inline void zox_gpu_disable_attribute(guint id) {
    if (id < vk_slots_max) {
        vk_ctx->attributes[id].enabled = 0;
    }
}

static inline void zox_gpu_enable_attribute(guint id) {
    if (id < vk_slots_max) {
        vk_ctx->attributes[id].enabled = 1;
    }
}

static inline void zox_gpu_attribute_divisor(guint id, guint value) {
    if (id < vk_slots_max) {
        vk_ctx->attributes[id].divisor = value;
    }
}

static inline void zox_gpu_set_attribute_floatx(guint id, const void* ptr, byte length) {
    const VkFormat formats[4] = { VK_FORMAT_R32_SFLOAT, VK_FORMAT_R32G32_SFLOAT, VK_FORMAT_R32G32B32_SFLOAT,
        VK_FORMAT_R32G32B32A32_SFLOAT };
    vulkan_attribute(id, formats[(length - 1) & 3], 0, ptr);
}

static inline void zox_gpu_set_attribute_float2(guint id, const void* ptr) {
    vulkan_attribute(id, VK_FORMAT_R32G32_SFLOAT, 0, ptr);
}

static inline void zox_gpu_enable_attribute_float2(guint id) {
    zox_gpu_enable_attribute(id);
    zox_gpu_set_attribute_float2(id, NULL);
}

static inline void zox_gpu_enable_set_attribute_float2(guint id, const void* ptr) {
    zox_gpu_enable_attribute(id);
    zox_gpu_set_attribute_float2(id, ptr);
}

static inline void zox_gpu_set_attribute_float3(guint id, const void* ptr) {
    vulkan_attribute(id, VK_FORMAT_R32G32B32_SFLOAT, 0, ptr);
}

static inline void zox_gpu_enable_attribute_float3(guint id, const void* ptr) {
    zox_gpu_enable_attribute(id);
    zox_gpu_set_attribute_float3(id, ptr);
}

static inline void zox_gpu_set_attribute_color(guint id, const void* ptr) {
    vulkan_attribute(id, VK_FORMAT_R8G8B8A8_UNORM, 0, ptr);
}

static inline void zox_gpu_set_attribute_float3_strided(guint id, gsizei stride, gsize offset) {
    vulkan_attribute(id, VK_FORMAT_R32G32B32_SFLOAT, stride, (const void*) offset);
}

// a buffer as an attribute (and its enabling), as gl's helpers do
static inline void vulkan_buffer_attribute(guint location, guint buffer, VkFormat format, uint stride) {
    vk_ctx->array_bound = buffer;
    zox_gpu_enable_attribute(location);
    vulkan_attribute(location, format, stride, NULL);
    vk_ctx->array_bound = 0;
}

static inline void opengl_enable_vertex_buffer(guint shader_index, guint vertex_buffer) {
    vulkan_buffer_attribute(shader_index, vertex_buffer, VK_FORMAT_R32G32B32_SFLOAT, 0);
}

static inline void opengl_enable_uv_buffer(guint shader_index, guint uv_buffer) {
    vulkan_buffer_attribute(shader_index, uv_buffer, VK_FORMAT_R32G32_SFLOAT, 0);
}

static inline void opengl_enable_color_buffer(guint shader_index, guint color_buffer) {
    vulkan_buffer_attribute(shader_index, color_buffer, vulkan_rgb8_format(), 3);
}

static inline void zox_gpu_array_buffer_byte(guint shader_index, guint buffer) {
    vulkan_buffer_attribute(shader_index, buffer, VK_FORMAT_R8_UNORM, 0);
}

static inline void zox_gpu_array_buffer_byte4(guint shader_index, guint buffer) {
    vulkan_buffer_attribute(shader_index, buffer, VK_FORMAT_R8G8B8A8_UNORM, 0);
}

// a vertex format's bytes (a stride of 0 is gl's tightly packed)
uint vulkan_format_size(VkFormat format) {
    switch (format) {
        case VK_FORMAT_R8_UNORM: return 1;
        case VK_FORMAT_R8G8B8_UNORM: return 3;
        case VK_FORMAT_R8G8B8A8_UNORM: return 4;
        case VK_FORMAT_R32_SFLOAT: return 4;
        case VK_FORMAT_R32G32_SFLOAT: return 8;
        case VK_FORMAT_R32G32B32_SFLOAT: return 12;
        default: return 16;
    }
}
