// one device and one window: a swapchain, or in --background runs (tests) an offscreen image, no surface
#define vk_frames_max 2     // frames in flight
#define vk_images_max 8

VkInstance vk_instance;
VkDebugUtilsMessengerEXT vk_messenger;
VkSurfaceKHR vk_surface;
VkPhysicalDevice vk_physical;
VkDevice vk_device;
VkQueue vk_queue;
uint vk_queue_family;
byte vk_on_gpu;                 // not lavapipe (software)

// the target, its images with one depth buffer
VkSwapchainKHR vk_swapchain;
VkFormat vk_color_format;
VkFormat vk_depth_format;
VkImageLayout vk_final_layout;  // the color image's layout after a render pass
byte vk_readable;               // its images can be copied (read back)
VkExtent2D vk_extent;
uint vk_images_count;
VkImage vk_images[vk_images_max];
VkImageView vk_views[vk_images_max];
VkFramebuffer vk_framebuffers[vk_images_max];
VkSemaphore vk_rendered[vk_images_max];   // signalled by an image's last submit, its present waits on it
VkDeviceMemory vk_offscreen_memory;
VkImage vk_depth;
VkDeviceMemory vk_depth_memory;
VkImageView vk_depth_view;
VkRenderPass vk_pass_clear;     // opens a frame, clearing it
VkRenderPass vk_pass_load;      // carries on a frame after a read back

// frames in flight
VkCommandPool vk_pool;
VkCommandBuffer vk_commands[vk_frames_max];
VkFence vk_fences[vk_frames_max];
VkSemaphore vk_acquired[vk_frames_max];
byte vk_frame;                  // which frame in flight
uint vk_image;                  // the image drawn to this frame
byte vk_recording;              // the frame's render pass is open
byte vk_submitted;              // read back mid frame: submitted, the image still ours, not presented
byte vk_resized;                // the swapchain is remade before the next frame
byte vk_vsync = 1;              // fifo, else mailbox or immediate (the app's vsync setting)
int2 vk_window_size;            // when the surface doesn't say its size (wayland)
float4 vk_clear_color = { 0, 0, 0, 1 };

// counts, for tests and logs
ulong vk_frames_presented;
int vk_validation_errors;

// submissions are numbered: what's recorded now is submitted as vk_serial_submitted + 1, a resource remembers the
// last one that read it, and is free to change once vk_serial_done passes it
ulong vk_serial_submitted;
ulong vk_serial_done;
ulong vk_fence_serial[vk_frames_max];
byte vk_frame_waited;           // this frame in flight's fence waited (its ring and pools free again)
ulong vk_ring_epoch;            // counts frames waited, a copy into the ring is good while it matches

// uploads (textures) recorded apart from the frame, submitted before it
VkCommandBuffer vk_uploads[vk_frames_max];
byte vk_uploading;

// per frame: a ring of host memory for uniforms and client vertex arrays, descriptor pools
#define vk_ring_size (4 << 20)
#define vk_rings_max 16
#define vk_pools_max 16
VkBuffer vk_rings[vk_frames_max][vk_rings_max];
VkDeviceMemory vk_rings_memory[vk_frames_max][vk_rings_max];
byte* vk_rings_mapped[vk_frames_max][vk_rings_max];
int vk_rings_count[vk_frames_max];
int vk_ring_index;              // the ring being filled this frame
VkDeviceSize vk_ring_used;
VkDescriptorPool vk_pools[vk_frames_max][vk_pools_max];
int vk_pools_count[vk_frames_max];
int vk_pool_index;
VkDeviceSize vk_uniform_align;

// the tables (dat/_.c)
typedef struct {
    byte* data;
    uint count;
    uint capacity;
    uint* free;
    uint free_count;
    size_t size;
} vk_table;
vk_table vk_buffers = { .size = sizeof(vk_buffer) };
vk_table vk_textures = { .size = sizeof(vk_texture) };
vk_table vk_shaders = { .size = sizeof(vk_shader) };
vk_table vk_programs = { .size = sizeof(vk_program) };

// gl's bound state
uint vk_program_bound;
uint vk_array_bound;
uint vk_element_bound;
uint vk_texture_bound;
vk_attribute vk_attributes[vk_slots_max];
uint vk_ubo_points[vk_slots_max];       // gl's uniform buffer binding points, the buffer at each
byte vk_blend;
byte vk_depth_test;
byte vk_depth_write = 1;
byte vk_cull;
byte vk_color_write = 1;
byte vk_bias_fill;
byte vk_bias_lines;
float vk_bias_factor;
float vk_bias_units;
float vk_line_width = 1;
float4 vk_viewport;             // gl's: x, y from the bottom left, width, height
byte vk_viewport_set;           // else the whole target

// what the command buffer has bound, set again when it changes
VkPipeline vk_pipeline_bound;
byte vk_dynamic_dirty;

// fallbacks: a zero vertex buffer (inputs with no attribute), a black texture, the sampler
VkBuffer vk_zeros;
VkDeviceMemory vk_zeros_memory;
uint vk_texture_black;
VkSampler vk_sampler;
byte vk_wide_lines;
byte vk_rgb8_vertex;            // R8G8B8 vertex format supported, else it's read as R8G8B8A8
