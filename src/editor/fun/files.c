static byte file_path_separator(char c) {
#ifdef zox_windows
    return c == '/' || c == '\\';
#else
    return c == '/';
#endif
}

static FolderPath file_window_path(const char* path) {
    FolderPath folder = { 0 };
    if (path && strlen(path) < sizeof(folder.value)) {
        strcpy(folder.value, path);
        size_t length = strlen(folder.value);
        size_t root_length = 1;
#ifdef zox_windows
        if (length >= 2 && folder.value[1] == ':') {
            root_length = 3;
        }
#endif
        while (length > root_length && file_path_separator(folder.value[length - 1])) {
            folder.value[--length] = '\0';
        }
    }
    return folder;
}

static FolderPath file_window_saves(void) {
    FolderPath saves = { 0 };
    get_save_directory(game_name, saves.value, sizeof(saves.value));
    return file_window_path(saves.value);
}

static const char* file_window_basename(const char* path) {
    const char* name = path;
    for (const char* c = path; *c; c++) {
        if (file_path_separator(*c)) {
            name = c + 1;
        }
    }
    return name;
}

static FolderPath file_window_parent(FolderPath folder) {
    const FolderPath resources = file_window_path(resources_path);
    const FolderPath saves = file_window_saves();
    if (!strcmp(folder.value, resources.value) || !strcmp(folder.value, saves.value)) {
        return (FolderPath) { 0 };
    }
    const size_t offset = file_window_basename(folder.value) - folder.value;
    if (offset == strlen(folder.value)) {
        return (FolderPath) { 0 }; // filesystem root
    }
    folder.value[offset] = '\0';
    return file_window_path(folder.value);
}
