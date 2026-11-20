// Recording on worker threads: a render system marked parallel (RenderParallel) is split across vk_workers_count
// threads, each running its part of the system's entities (zox_run_part, on a flecs stage of its own) into a
// secondary command buffer with its own context; the main thread waits, then replays them in order
//  - engine note: threads of the vulkan module's own, beside flecs' workers. camera_render3_system is single
//    threaded, so the world is read only and the flecs workers wait while these use their stages (1 up)
//  - a part starts from the main thread's gl state (vk_contexts[0]), its uniforms copied once a split
//  - what a part changes (spawns, sets) is deferred on its stage, as a multi threaded system's would be

typedef struct {
    ecs* stage;
    entity system;
    int index;
    int count;
    VkCommandBuffer commands;
} vk_part;

#define vk_workers_max 8
pthread_t vk_workers[vk_workers_max];
int vk_workers_count;
vk_part vk_parts[vk_workers_max];
pthread_mutex_t vk_parts_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t vk_parts_start = PTHREAD_COND_INITIALIZER;
pthread_cond_t vk_parts_done = PTHREAD_COND_INITIALIZER;
ulong vk_parts_round;
int vk_parts_pending;
byte vk_workers_quit;
byte vk_workers_started;

// a context's next secondary command buffer this frame (made the first time)
VkCommandBuffer vulkan_secondary(vk_context* context) {
    if (!context->command_pools[vk_frame]) {
        const VkCommandPoolCreateInfo pool = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
            .queueFamilyIndex = vk_queue_family,
        };
        vkCreateCommandPool(vk_device, &pool, NULL, &context->command_pools[vk_frame]);
    }
    if (context->secondary_index >= context->secondaries_count[vk_frame]) {
        const uint count = context->secondaries_count[vk_frame] + 1;
        context->secondaries[vk_frame] = realloc(context->secondaries[vk_frame], sizeof(VkCommandBuffer) * count);
        const VkCommandBufferAllocateInfo allocate = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = context->command_pools[vk_frame],
            .level = VK_COMMAND_BUFFER_LEVEL_SECONDARY,
            .commandBufferCount = 1,
        };
        vkAllocateCommandBuffers(vk_device, &allocate, &context->secondaries[vk_frame][count - 1]);
        context->secondaries_count[vk_frame] = count;
    }
    return context->secondaries[vk_frame][context->secondary_index++];
}

// a worker's part: the main thread's gl state, recorded into its secondary buffer
void vulkan_record_part(const vk_part* part) {
    vk_context* context = vk_ctx;
    memcpy(context, &vk_contexts[0], offsetof(vk_context, commands));
    context->commands = part->commands;
    context->pipeline_bound = VK_NULL_HANDLE;
    context->dynamic_dirty = 1;
    const VkCommandBufferInheritanceInfo inherit = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
        .renderPass = vulkan_target_load_pass(),
        .subpass = 0,
        .framebuffer = vk_target_framebuffer,
    };
    const VkCommandBufferBeginInfo begin = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT,
        .pInheritanceInfo = &inherit,
    };
    vkBeginCommandBuffer(part->commands, &begin);
    zox_run_part(part->stage, part->system, part->index, part->count);
    vkEndCommandBuffer(part->commands);
}

void* vulkan_worker(void* arg) {
    const int index = (int) (size_t) arg;
    vk_ctx = &vk_contexts[1 + index];
    ulong seen = 0;
    while (1) {
        pthread_mutex_lock(&vk_parts_lock);
        while (vk_parts_round == seen && !vk_workers_quit) {
            pthread_cond_wait(&vk_parts_start, &vk_parts_lock);
        }
        if (vk_workers_quit) {
            pthread_mutex_unlock(&vk_parts_lock);
            return NULL;
        }
        seen = vk_parts_round;
        const vk_part part = vk_parts[index];
        pthread_mutex_unlock(&vk_parts_lock);
        vulkan_record_part(&part);
        pthread_mutex_lock(&vk_parts_lock);
        if (--vk_parts_pending == 0) {
            pthread_cond_signal(&vk_parts_done);
        }
        pthread_mutex_unlock(&vk_parts_lock);
    }
}

// the threads, one less than the world's stages (the main thread has stage 0), at most vk_workers_max
void vulkan_start_workers(ecs* world) {
    const int stages = zox_stage_count(world);
    vk_workers_count = stages - 1 < vk_workers_max ? stages - 1 : vk_workers_max;
    // ZOX_VK_WORKERS=n: at most n (0 records on the main thread only), i.e. to compare or debug
    const char* most = getenv("ZOX_VK_WORKERS");
    if (most && atoi(most) < vk_workers_count) {
        vk_workers_count = atoi(most) > 0 ? atoi(most) : 0;
    }
    for (int i = 0; i < vk_workers_count; i++) {
        vk_contexts[1 + i].line_width = 1;
        pthread_create(&vk_workers[i], NULL, vulkan_worker, (void*) (size_t) i);
    }
    zox_log("+ vulkan recording workers [%i]", vk_workers_count);
}

void vulkan_stop_workers() {
    pthread_mutex_lock(&vk_parts_lock);
    vk_workers_quit = 1;
    pthread_cond_broadcast(&vk_parts_start);
    pthread_mutex_unlock(&vk_parts_lock);
    for (int i = 0; i < vk_workers_count; i++) {
        pthread_join(vk_workers[i], NULL);
    }
    vk_workers_count = 0;
}

// a render system: split across the workers when it's parallel, else on this thread as gl does
static inline void zox_gpu_run_system(ecs* world, entity system, byte parallel) {
    ecs* real = zox_world_of(world);
    if (parallel && !vk_workers_started) {
        vk_workers_started = 1;
        vulkan_start_workers(real);
    }
    if (!parallel || vk_workers_count < 2 || !vulkan_frame_begin()) {
        zox_run(world, system);
        return;
    }
    vk_job++;
    VkCommandBuffer buffers[vk_workers_max];
    pthread_mutex_lock(&vk_parts_lock);
    for (int i = 0; i < vk_workers_count; i++) {
        buffers[i] = vulkan_secondary(&vk_contexts[1 + i]);
        vk_parts[i] = (vk_part) { zox_stage(real, 1 + i), system, i, vk_workers_count, buffers[i] };
    }
    vk_parts_pending = vk_workers_count;
    vk_parts_round++;
    pthread_cond_broadcast(&vk_parts_start);
    while (vk_parts_pending) {
        pthread_cond_wait(&vk_parts_done, &vk_parts_lock);
    }
    pthread_mutex_unlock(&vk_parts_lock);
    // secondaries play in a pass of their own (a pass can't mix them with draws), then the main thread's carries on
    const VkCommandBuffer commands = vk_contexts[0].commands;
    vkCmdEndRenderPass(commands);
    const VkRenderPass carry_on = vulkan_target_load_pass();
    vulkan_pass_begin(commands, carry_on, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
    vkCmdExecuteCommands(commands, vk_workers_count, buffers);
    vkCmdEndRenderPass(commands);
    vulkan_pass_begin(commands, carry_on, VK_SUBPASS_CONTENTS_INLINE);
    vk_contexts[0].pipeline_bound = VK_NULL_HANDLE;
    vk_contexts[0].dynamic_dirty = 1;
}
