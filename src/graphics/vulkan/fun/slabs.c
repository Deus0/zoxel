// Slabs: buffers carved from big host visible blocks, so a mesh upload doesn't allocate gpu memory (vkAllocateMemory
// is a slow driver call, it took ~29% of the main thread streaming far terrain)
//  - power of two size classes, 256 bytes up; a class's blocks are 4MB (or one slot when slots are bigger)
//  - an allocation is a slot: its block's buffer, an offset into it and where that's mapped
//  - its handle is its kind, class and slot (+1, 0 is none), freed slots are reused
//  - two kinds: host visible (mapped, written by the cpu) and gpu only (meshes where the gpu's memory isn't host
//    visible, vk_meshes_staged: written by a copy from a host slot, fun/buffers.c)

#define vk_slab_min 8           // 256 bytes
#define vk_slab_max 28          // 256MB
#define vk_slab_block (4 << 20)

typedef struct {
    VkBuffer buffer;
    VkDeviceMemory memory;
    byte* mapped;
} vk_slab_block_t;

typedef struct {
    vk_slab_block_t* blocks;
    uint blocks_count;
    uint* free;
    uint free_count;
    uint free_capacity;
    uint next;                  // slots handed out so far
} vk_slab_class;

typedef struct {
    VkBuffer buffer;
    VkDeviceSize offset;
    byte* mapped;
    VkDeviceSize size;          // the slot's
    uint handle;
} vk_allocation;

vk_slab_class vk_slabs[2][vk_slab_max + 1];    // [gpu only]

static inline VkDeviceSize vk_slab_size(uint class) {
    return (VkDeviceSize) 1 << class;
}

static inline uint vk_slab_per_block(uint class) {
    return vk_slab_size(class) >= vk_slab_block ? 1 : vk_slab_block / vk_slab_size(class);
}

// a slot of at least bytes; handle 0 if it couldn't
vk_allocation vulkan_slab_allocate_locked(VkDeviceSize bytes, byte device) {
    vk_allocation allocation = { 0 };
    uint class = vk_slab_min;
    while (class <= vk_slab_max && vk_slab_size(class) < bytes) {
        class++;
    }
    if (class > vk_slab_max) {
        zox_loge("[vulkan] a buffer of [%lu] bytes is over the slabs' biggest", (ulong) bytes);
        return allocation;
    }
    vk_slab_class* slabs = &vk_slabs[device][class];
    const uint per_block = vk_slab_per_block(class);
    uint slot;
    if (slabs->free_count) {
        slot = slabs->free[--slabs->free_count];
    } else {
        slot = slabs->next++;
        if (slot / per_block >= slabs->blocks_count) {
            slabs->blocks = realloc(slabs->blocks, sizeof(vk_slab_block_t) * (slabs->blocks_count + 1));
            vk_slab_block_t* block = &slabs->blocks[slabs->blocks_count];
            const VkBufferUsageFlags usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT
                | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
            const VkDeviceSize size = per_block * vk_slab_size(class);
            if (!(device ? vulkan_create_device_buffer(size, usage, &block->buffer, &block->memory)
                : vulkan_create_host_buffer(size, usage, &block->buffer, &block->memory)))
            {
                slabs->next--;
                return allocation;
            }
            block->mapped = NULL;
            if (!device) {
                vkMapMemory(vk_device, block->memory, 0, VK_WHOLE_SIZE, 0, (void**) &block->mapped);
            }
            slabs->blocks_count++;
        }
    }
    const vk_slab_block_t* block = &slabs->blocks[slot / per_block];
    allocation.buffer = block->buffer;
    allocation.offset = (slot % per_block) * vk_slab_size(class);
    allocation.mapped = block->mapped ? block->mapped + allocation.offset : NULL;
    allocation.size = vk_slab_size(class);
    allocation.handle = ((uint) device << 30 | class << 24 | slot) + 1;
    return allocation;
}

// recording workers upload too (particles), so slots are taken and freed under vk_lock
vk_allocation vulkan_slab_allocate(VkDeviceSize bytes, byte device) {
    pthread_mutex_lock(&vk_lock);
    const vk_allocation allocation = vulkan_slab_allocate_locked(bytes, device);
    pthread_mutex_unlock(&vk_lock);
    return allocation;
}

void vulkan_slab_free(uint handle) {
    if (!handle) {
        return;
    }
    pthread_mutex_lock(&vk_lock);
    handle--;
    vk_slab_class* slabs = &vk_slabs[handle >> 30][(handle >> 24) & 63];

    if (slabs->free_count == slabs->free_capacity) {
        slabs->free_capacity = slabs->free_capacity ? slabs->free_capacity * 2 : 64;
        slabs->free = realloc(slabs->free, sizeof(uint) * slabs->free_capacity);
    }
    slabs->free[slabs->free_count++] = handle & 0xFFFFFF;
    pthread_mutex_unlock(&vk_lock);
}

void vulkan_dispose_slabs() {
    for (uint c = 0; c < 2 * (vk_slab_max + 1); c++) {
        vk_slab_class* slabs = &vk_slabs[c / (vk_slab_max + 1)][c % (vk_slab_max + 1)];
        for (uint b = 0; b < slabs->blocks_count; b++) {
            vkDestroyBuffer(vk_device, slabs->blocks[b].buffer, NULL);
            vkFreeMemory(vk_device, slabs->blocks[b].memory, NULL);
        }
        free(slabs->blocks);
        free(slabs->free);
        *slabs = (vk_slab_class) { 0 };
    }
}
