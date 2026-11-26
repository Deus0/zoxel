// one device and one window: a swapchain, or in --background runs (tests) an offscreen image, no surface
#define vk_images_max 8

VkInstance vk_instance;
VkDebugUtilsMessengerEXT vk_messenger;
VkSurfaceKHR vk_surface;
VkPhysicalDevice vk_physical;
VkDevice vk_device;
VkQueue vk_queue;
uint vk_queue_family;
// another api making the instance and device instead (OpenXR's XR_KHR_vulkan_enable2, xr/openxr/vulkan.c): it adds
//  the extensions it needs; unset, the backend makes its own
VkResult (*vk_create_instance_with)(const VkInstanceCreateInfo* info, VkInstance* instance) = NULL;
VkResult (*vk_pick_physical_with)(VkPhysicalDevice* physical) = NULL;
VkResult (*vk_create_device_with)(VkPhysicalDevice physical, const VkDeviceCreateInfo* info, VkDevice* device) = NULL;
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
VkRenderPass vk_pass_texture_begin[2]; // a render texture's (fun/targets.c): [0] bytes, [1] half floats (hdr)
VkRenderPass vk_pass_texture_load[2];

// what's drawn to now: the frame's image, or a render texture (an fbo bound, its passes and pipelines its own)
uint vk_fbo_bound;
byte vk_target_texture;
byte vk_target_external;    // an external target's bound (OpenXR's eyes, fun/external.c)
VkRenderPass vk_pass_external;          // an external target's, begun: its depth cleared
VkRenderPass vk_pass_external_load;     // carried on after a split's secondaries (workers.c)
VkFramebuffer vk_target_framebuffer;
VkExtent2D vk_target_extent;

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
byte vk_extent_made;            // a swapchain was made, its size not yet given to the game (vulkan_take_extent)
byte vk_surface_lost;           // android took the window's surface (the app in the background): no frames
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

uint vk_ubo_points[vk_slots_max];       // gl's uniform buffer binding points, the buffer at each

// each thread's context: the main thread's first, then the recording workers' (fun/workers.c)
#define vk_contexts_max 17
vk_context vk_contexts[vk_contexts_max];
static __thread vk_context* vk_ctx = &vk_contexts[0];
ulong vk_job;                   // counts split runs (vulkan_run_split), a worker resyncs its uniforms each
pthread_mutex_t vk_lock = PTHREAD_MUTEX_INITIALIZER;   // what the workers share: slabs, garbage, pipelines

// fallbacks: a zero vertex buffer (inputs with no attribute), a black texture, the sampler
VkBuffer vk_zeros;
VkDeviceMemory vk_zeros_memory;
uint vk_texture_black;
VkSampler vk_sampler;
byte vk_wide_lines;
byte vk_outlines;       // independentBlend: render textures get an outline mask (an integer target beside the blended)
byte vk_meshes_staged;          // meshes in gpu only memory, written by copies (vulkan_pick_meshes_staged)
byte vk_rgb8_vertex;            // R8G8B8 vertex format supported, else it's read as R8G8B8A8
