// What the gpu may still read is destroyed once the submission that read it is done (gl does this for us)

typedef struct {
    VkBuffer buffer;
    VkImage image;
    VkImageView view;
    VkDeviceMemory memory;
    uint allocation;            // a slab slot
    ulong serial;
} vk_garbage_item;

vk_garbage_item* vk_garbage;
int vk_garbage_count;
int vk_garbage_capacity;

static inline ulong vk_serial_recording() {
    return vk_serial_submitted + 1;
}

// a resource last read by this submission can't change yet
static inline byte vk_busy(ulong used) {
    return used > vk_serial_done;
}

void vulkan_destroy(vk_garbage_item item) {
    vkDestroyImageView(vk_device, item.view, NULL);
    vkDestroyImage(vk_device, item.image, NULL);
    vkDestroyBuffer(vk_device, item.buffer, NULL);
    vkFreeMemory(vk_device, item.memory, NULL);
    vulkan_slab_free(item.allocation);
}

// destroyed now if nothing reads it, else after serial is done
void vulkan_garbage_add(vk_garbage_item item) {
    if (!vk_busy(item.serial)) {
        vulkan_destroy(item);
        return;
    }
    pthread_mutex_lock(&vk_lock);
    if (vk_garbage_count == vk_garbage_capacity) {
        vk_garbage_capacity = vk_garbage_capacity ? vk_garbage_capacity * 2 : 256;
        vk_garbage = realloc(vk_garbage, sizeof(vk_garbage_item) * vk_garbage_capacity);
    }
    vk_garbage[vk_garbage_count++] = item;
    pthread_mutex_unlock(&vk_lock);
}

void vulkan_garbage_collect() {
    int kept = 0;
    for (int i = 0; i < vk_garbage_count; i++) {
        if (vk_busy(vk_garbage[i].serial)) {
            vk_garbage[kept++] = vk_garbage[i];
        } else {
            vulkan_destroy(vk_garbage[i]);
        }
    }
    vk_garbage_count = kept;
}

void vulkan_garbage_dispose() {
    vk_serial_done = vk_serial_submitted;
    vulkan_garbage_collect();
    free(vk_garbage);
    vk_garbage = NULL;
    vk_garbage_capacity = 0;
}
