// NOTE: Begin Saving to a File!
zox_sys2(SaveBeginSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(FolderPathLink);
    zox_sys_in(SaveGamePath);
    zox_sys_out(FileLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(FolderPathLink, folder);
        zox_sys_i(SaveGamePath, path);
        zox_sys_o(FileLink, link);
        if (!zox_valid(folder->value) || !zox_has(folder->value, FolderPath)) {
            zox_logw("Folder Entity Invalid [%s]", zox_get_name(e));
            continue;
        }
        zox_geter(folder->value, FolderPath, folder_path);
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
} zox_sys_end(SaveBeginSystem);
