#include "save.c"
#include "load.c"

void zox_systems_settings(ecs* world) {
    zox_system(
        setting_save_system,
        zoxp_update,
        [in] settings.SettingDirty,
        [none] settings.Setting
    );
    // Can we take in game name from App??
    zox_system(
        settings_load_system,
        zoxp_update,
        [in] core.ZoxName,
        [out] settings.LoadSettings,
        [none] !core.PreInitialize,
        [none] !core.Initialize,
    );
}
