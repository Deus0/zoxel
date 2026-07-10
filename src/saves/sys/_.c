// Maybe a DebugSaveSystem and DebugLoadSystem just to log the actions
#include "save_begin.c"

void zox_define_systems_saves(ecs* world) {
    zox_system(
        SaveBeginSystem,
        zoxp_update,
        [in] saves.SaveDirty,
        [in] saves.FolderPathLink,
        [in] saves.SaveGamePath,
        [out] saves.FileLink,
        [out] saves.Saving,
        [none] saves.Saver
    );
}
