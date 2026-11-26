// What the gl style calls make: buffers, textures, shaders and programs, each in a table, an id its index (0 none)

// a buffer: a slab's slot (fun/slabs.c), host visible and mapped (on integrated gpus that's device memory too)
typedef struct {
    VkBuffer buffer;            // the slab block's, the buffer's data from offset
    VkDeviceSize offset;
    uint allocation;            // the slot's handle
    byte* mapped;
    VkDeviceSize size;          // bytes written by the last upload
    VkDeviceSize capacity;
    ulong used;                 // the submission that last read it (vk_busy)
    byte host;                  // kept in host visible slabs (a storage buffer: read back), never staged
    byte* shadow;               // a uniform buffer's data, copied to the frame's ring when drawn
    VkDeviceSize shadow_used;   // the bytes its last write filled (only they're copied, the block's range reserved)
    byte shadow_dirty;
    VkBuffer ring;              // and where it was copied this frame
    uint ring_offset;
    ulong ring_epoch;
    byte live;
} vk_buffer;

typedef struct {
    VkImage image;
    VkDeviceMemory memory;
    VkImageView view;
    int2 size;
    ulong used;
    byte hdr;                   // a camera's half float colour target (vk_hdr_format), no pixels uploaded
    byte mips;                  // its levels, past 1 for bloom: made as its camera's pass ends (vulkan_texture_mips)
    VkImageView target_view;    // with mips, its first level's, what its framebuffer draws to (view samples them all)
    byte live;
} vk_texture;

// a name and a number: a shader's input, output, block or sampler, and the word of its decoration to patch
typedef struct {
    char name[48];
    int value;                  // location, or a block's size
    int word;                   // the Location or Binding decoration's literal in the spir-v
} vk_slot;

// a default uniform block member: its byte offset, and an array's stride
typedef struct {
    char name[48];
    int offset;
    int stride;
} vk_member;

#define vk_slots_max 16
#define vk_members_max 32

// a stage compiled to spir-v and what reflection found in it
typedef struct {
    uint* words;
    uint count;
    byte fragment;
    vk_member members[vk_members_max];  // its loose uniforms (the default block)
    int members_count;
    int block_size;
    int block_word;             // the default block's Binding literal, -1 if it has none
    vk_slot blocks[vk_slots_max];       // named uniform blocks (InstanceMatrices), value their size
    int blocks_count;
    vk_slot samplers[vk_slots_max];
    int samplers_count;
    vk_slot storages[vk_slots_max];     // storage buffers (compute), value their binding
    int storages_count;
    vk_slot inputs[vk_slots_max];
    int inputs_count;
    vk_slot outputs[vk_slots_max];
    int outputs_count;
    byte live;
} vk_shader;

// a uniform as gl's location: where it is in each stage's block (-1 if not there)
typedef struct {
    char name[48];
    int offset[2];
    int stride[2];
    int sampler;                // a sampler's index in its program's samplers (glUniform1i sets its unit), else -1
} vk_uniform;

// a named uniform block: its binding, size and the gl binding point (zox_gpu_ubo_block_binding) it reads
typedef struct {
    char name[48];
    int binding;
    int size;
    int point;
} vk_block;

// the state a pipeline's made of; zeroed before it's filled, compared with memcmp
typedef struct {
    byte topology;              // 0 triangles, 1 lines, 2 points
    byte target;                // a render texture's pass (its format): 1 bytes, 2 half floats (vk_target_texture)
    byte blend;
    byte depth_test;
    byte depth_write;
    byte cull;
    byte color_write;
    byte depth_bias;
    byte mask;                  // a render texture's outline mask written (colour attachment 1, doc/plans/outlines.md)
    byte inputs_count;
    struct {
        byte location;
        byte instanced;
        ushort stride;
        uint format;
    } inputs[vk_slots_max];
} vk_pipeline_key;

typedef struct {
    vk_pipeline_key key;
    VkPipeline pipeline;
} vk_pipeline;

typedef struct {
    VkShaderModule modules[2];
    VkDescriptorSetLayout set_layout;
    VkPipelineLayout layout;
    vk_uniform* uniforms;
    int uniforms_count;
    int data_size[2];           // each stage's default block's bytes (its values are each context's, vk_program_state)
    vk_block blocks[vk_slots_max];
    int blocks_count;
    int samplers[vk_slots_max];         // their bindings
    byte sampler_units[vk_slots_max];   // each one's texture unit (gl's glUniform1i on a sampler), 0 by default
    int samplers_count;
    vk_slot inputs[vk_slots_max];       // the vertex shader's inputs, value their location
    int inputs_count;
    vk_pipeline* pipelines;
    int pipelines_count;
    byte compute;               // a compute program: one stage, its pipeline made with it (fun/compute.c)
    VkPipeline compute_pipeline;
    int storages[vk_slots_max];         // its storage buffers' bindings
    int storages_count;
    byte live;
} vk_program;

// a program's uniform values as a context (a thread) set them, gl's per program uniforms
typedef struct {
    byte* data[2];              // each stage's default block, written by the uniform setters
    byte data_dirty[2];
    VkBuffer data_ring[2];      // where it was last copied this frame
    uint data_offset[2];
    ulong data_epoch[2];
    VkDescriptorSet set;        // the last set made this frame, reused while it matches
    ulong set_epoch;
    VkImageView set_views[vk_slots_max];    // each sampler's: a texture written again gets a new image, its view
    VkBuffer set_buffers[vk_slots_max];
    ulong job;                  // the job it was last synced in (a worker's copy of the main thread's values)
} vk_program_state;

// a vertex attribute as gl's glVertexAttribPointer left it; buffer 0 is a client pointer (copied at the draw)
typedef struct {
    byte enabled;
    uint buffer;
    const byte* pointer;        // the offset into the buffer, or the client memory
    VkFormat format;
    uint stride;
    uint divisor;
} vk_attribute;

#define vk_ring_size (4 << 20)
#define vk_rings_max 16
#define vk_pools_max 16
#define vk_frames_max 2

// what a thread records with: gl's bound state (as zox_gpu_* calls leave it), its command buffer, and per frame in
// flight its ring, descriptor pools and secondary command buffers; the main thread's is vk_contexts[0]
#define vk_units_max 4
#define vk_rbo_mask_bit 0x80000000     // a unit's id: an rbo's outline mask, not its depth

typedef struct {
    uint program_bound;
    uint array_bound;
    uint element_bound;
    uint texture_bound;
    uint units_bound[vk_units_max];     // units past 0: a render camera's depth or outline mask (vk_rbo_mask_bit)
    byte outline_mask;          // outlined models draw: their mask written (zox_gpu_outline_mask)
    vk_attribute attributes[vk_slots_max];
    byte blend;
    byte depth_test;
    byte depth_write;
    byte cull;
    byte color_write;
    byte bias_fill;
    byte bias_lines;
    float bias_factor;
    float bias_units;
    float line_width;
    float4 viewport;            // gl's: x, y from the bottom left, width, height
    int4 scissor;
    byte scissor_set;
    byte viewport_set;          // else the whole target
    // recording
    VkCommandBuffer commands;
    VkPipeline pipeline_bound;
    byte dynamic_dirty;
    vk_program_state* programs; // by program id
    uint programs_count;
    // per frame in flight
    VkBuffer rings[vk_frames_max][vk_rings_max];
    VkDeviceMemory rings_memory[vk_frames_max][vk_rings_max];
    byte* rings_mapped[vk_frames_max][vk_rings_max];
    int rings_count[vk_frames_max];
    int ring_index;
    VkDeviceSize ring_used;
    VkDescriptorPool pools[vk_frames_max][vk_pools_max];
    int pools_count[vk_frames_max];
    int pool_index;
    VkCommandPool command_pools[vk_frames_max];
    VkCommandBuffer* secondaries[vk_frames_max];
    uint secondaries_count[vk_frames_max];
    uint secondary_index;
} vk_context;
