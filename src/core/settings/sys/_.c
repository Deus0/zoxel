#include "save.c"
#include "load.c"

void define_systems_settings(ecs* world) {
    zox_system(
        SettingSaveSystem,
        zoxp_update,
        [in] settings.SettingDirty,
        [none] settings.Setting
    );
    // Can we take in game name from App??
    zox_system(
        SettingsLoadSystem,
        zoxp_update,
        [in] core.ZoxName,
        [out] settings.LoadSettings,
        [none] !core.PreInitialize,
        [none] !core.Initialize,
    );
}
