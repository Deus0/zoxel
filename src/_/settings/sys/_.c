#include "save.c"

void define_systems_settings(ecs* world) {
    zox_system(
        SettingSaveSystem,
        zoxp_update,
        [in] settings.SettingDirty,
        [none] settings.Setting
    );

}
