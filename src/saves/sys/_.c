// Maybe a DebugSaveSystem and DebugLoadSystem just to log the actions
#include "save_begin.c"

void zox_systems_saves(ecs* world) {
    zox_system(
        save_begin_system,
        zoxp_update,
        [in] saves.SaveGamePath,
        [out] saves.FileLink,
        [out] saves.Saving,
        [none] saves.SaveDirty,
        [none] saves.Saver,
    );
}
