// NOTE: Begin Saving to a File!
void SaveBeginSystem(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SaveGamePath);
    zox_sys_out(FileLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        entity folder = zox_get_link(world, e, FolderPathLink);
        if (!folder) {
            continue;   // NOTE: Only savers linked to a folder, i.e. a realm
        }
        zox_sys_i(SaveGamePath, path);
        zox_sys_o(FileLink, link);
        if (!zox_valid(folder) || !zox_has(folder, FolderPath)) {
            zox_logw("Folder Entity Invalid [%s]", zox_get_name(e));
            continue;
        }
        zox_geter(folder, FolderPath, folder_path);
        char* filepath = join_path(folder_path->value, path->value);
        if (dbg_log) {
            zox_log("Save Begins [%s] to [%s]", zox_get_name(e), filepath);
        }
        // FILE* file = fopen(filepath, "w");
        link->value = fopen(filepath, "w");
        free(filepath);
        zox_remove(e, SaveDirty);
        zox_add(e, Saving);
    }
    zox_sys_on_end();
} zoxd_system(SaveBeginSystem);
