#include "settings.c"

void zox_define_systems_apps(ecs *world) {
    zox_system_1(
        AppsSettingsSystem,
        zoxp_mainthread,
        [in] settings.LoadSettings
    );
    zox_system_1(
        AppsSettingsDirtySystem,
        zoxp_mainthread,
        [in] settings.SettingDirty,
        [in] core.ZoxName,
        [in] settings.Setting
    );
}
