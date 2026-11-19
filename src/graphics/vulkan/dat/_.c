// What the gl style calls make: buffers, textures, shaders and programs, each in a table, an id its index (0 none)

// a buffer, host visible and mapped (on integrated gpus that's device memory too)
typedef struct {
    VkBuffer buffer;
    VkDeviceMemory memory;
    byte* mapped;
    VkDeviceSize size;          // bytes written by the last upload
    VkDeviceSize capacity;
    ulong used;                 // the submission that last read it (vk_busy)
    byte* shadow;               // a uniform buffer's data, copied to the frame's ring when drawn
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
    byte blend;
    byte depth_test;
    byte depth_write;
    byte cull;
    byte color_write;
    byte depth_bias;
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
    byte* data[2];              // each stage's default block, written by the uniform setters
    int data_size[2];
    byte data_dirty[2];
    VkBuffer data_ring[2];      // where it was last copied this frame
    uint data_offset[2];
    ulong data_epoch[2];
    vk_block blocks[vk_slots_max];
    int blocks_count;
    int samplers[vk_slots_max];         // their bindings
    int samplers_count;
    vk_slot inputs[vk_slots_max];       // the vertex shader's inputs, value their location
    int inputs_count;
    vk_pipeline* pipelines;
    int pipelines_count;
    VkDescriptorSet set;        // the last set made this frame, reused while it matches
    ulong set_epoch;
    uint set_texture;
    VkBuffer set_buffers[vk_slots_max];
    byte live;
} vk_program;

// a vertex attribute as gl's glVertexAttribPointer left it; buffer 0 is a client pointer (copied at the draw)
typedef struct {
    byte enabled;
    uint buffer;
    const byte* pointer;        // the offset into the buffer, or the client memory
    VkFormat format;
    uint stride;
    uint divisor;
} vk_attribute;
