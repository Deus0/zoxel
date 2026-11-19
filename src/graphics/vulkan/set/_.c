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
int2 vk_window_size;            // when the surface doesn't say its size (wayland)
float4 vk_clear_color = { 0, 0, 0, 1 };

// counts, for tests and logs
ulong vk_frames_presented;
int vk_validation_errors;
