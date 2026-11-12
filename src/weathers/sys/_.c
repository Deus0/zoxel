#include "skybox_restore_system.c"
#include "skybox_set_time_system.c"

void zox_systems_weather(ecs* world) {
    zox_gpu_restore_system(
        skybox_restore_system,
        [in] rendering.MaterialGPULink,
        [in] colorz.ColorRGB,
        [in] colorz.SecondaryColorRGB,
        [none] Skybox
    );
    zox_system_1(
        skybox_set_time_system,
        zoxp_gpu_upload,
        [in] rendering.MaterialGPULink,
        [none] Skybox
    );
}
