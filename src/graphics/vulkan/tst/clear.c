// vulkan_clear: the vulkan backend draws frames, clears them and reads them back
//  - ready once a few frames are presented (the swapchain, or offscreen in --background)
//  - clears the frame to blue and reads it back, then a rect at gl's bottom left to red (it carries the frame on
//      after the read back, vk_pass_load) and reads again: the rect at the bottom of the image, the blue kept
//  - no validation errors (debug builds turn the layer on when it's installed)

byte zox_tst_vulkan_clear_ready(ecs* world) {
    (void) world;
    return vk_frames_presented >= 3;
}

// 1 if the pixel at x, y (from the top) is near r, g, b
byte zox_tst_vulkan_pixel_is(const byte* pixels, int2 size, int x, int y, byte3 rgb) {
    const byte* p = &pixels[(y * size.x + x) * 3];
    return abs(p[0] - rgb.x) <= 1 && abs(p[1] - rgb.y) <= 1 && abs(p[2] - rgb.z) <= 1;
}

void zox_tst_vulkan_clear_check(ecs* world) {
    (void) world;
    zox_tst_check(vk_device, "no vulkan device");
    const int2 size = { vk_extent.width, vk_extent.height };
    zox_tst_check(size.x > 0 && size.y > 0, "no target [%ix%i]", size.x, size.y);
    const float4 before = vk_clear_color;
    const byte3 blue = { 51, 102, 153 };
    const byte3 red = { 255, 0, 0 };
    zox_gpu_set_clear_color((float4) { 0.2f, 0.4f, 0.6f, 1 });
    zox_gpu_clear_viewport();
    byte* pixels = zox_gpu_read_screen(size);
    zox_tst_check(pixels, "read back failed");
    if (pixels) {
        zox_tst_check(zox_tst_vulkan_pixel_is(pixels, size, size.x / 2, size.y / 2, blue),
            "the middle isn't the clear color");
        free(pixels);
    }
    zox_gpu_set_clear_color((float4) { 1, 0, 0, 1 });
    zox_gpu_clear_rect(float2_zero, (float2) { size.x / 2, size.y / 2 });
    pixels = zox_gpu_read_screen(size);
    zox_tst_check(pixels, "second read back failed");
    if (pixels) {
        zox_tst_check(zox_tst_vulkan_pixel_is(pixels, size, size.x / 4, size.y * 3 / 4, red),
            "the rect isn't at the bottom left");
        zox_tst_check(zox_tst_vulkan_pixel_is(pixels, size, size.x * 3 / 4, size.y / 4, blue),
            "the clear before the read back was lost");
        free(pixels);
    }
    zox_gpu_set_clear_color(before);
    zox_tst_check(!vk_validation_errors, "[%i] validation errors", vk_validation_errors);
    zox_log("[tst] vulkan [%s] target [%ix%i] frames [%lu]", vk_surface ? "swapchain" : "offscreen",
        size.x, size.y, vk_frames_presented);
}
