#include "skybox_restore_system.c"
#include "skybox_set_time_system.c"

void define_systems_weather(ecs* world) {
    zox_gpu_restore_system(
        SkyboxRestoreSystem,
        [in] rendering.MaterialGPULink,
        [in] colorz.ColorRGB,
        [in] colorz.SecondaryColorRGB,
        [none] Skybox
    );
    zox_system_1(
        SkyboxSetTimeSystem,
        EcsOnUpdate,
        [in] rendering.MaterialGPULink,
        [none] Skybox
    );
}
