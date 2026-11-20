#include "clear.c"
#include "shaders.c"

void zox_tsts_vulkan() {
    zox_tst_add("vulkan_clear", zox_tst_vulkan_clear_ready, zox_tst_vulkan_clear_check, 60);
    zox_tst_add("vulkan_shaders", zox_tst_vulkan_shaders_ready, zox_tst_vulkan_shaders_check, 60);
}
