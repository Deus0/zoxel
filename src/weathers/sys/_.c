#include "skybox_restore_system.c"
#include "skybox_set_time_system.c"
#include "game.c"

void zox_systems_weather(ecs* world) {
    zox_system(
        WeatherGameStateSystem,
        zoxp_update,
        [in] games.GameStateDirty,
        [in] games.GameState,
        [none] games.Game
    );
    zox_gpu_restore_system(
        SkyboxRestoreSystem,
        [in] rendering.MaterialGPULink,
        [in] colorz.ColorRGB,
        [in] colorz.SecondaryColorRGB,
        [none] Skybox
    );
    zox_system_1(
        SkyboxSetTimeSystem,
        zoxp_gpu_upload,
        [in] rendering.MaterialGPULink,
        [none] Skybox
    );
}
