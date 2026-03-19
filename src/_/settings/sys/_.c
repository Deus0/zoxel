#include "save.c"

void define_systems_settings(ecs* world) {

    zox_system(
        SettingSaveSystem,
        EcsOnUpdate,
        [in] settings.SettingDirty,
        [none] settings.Setting
    );

}
