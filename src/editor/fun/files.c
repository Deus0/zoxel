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
    if (!strcmp(folder.value, resources.value)) {
        return (FolderPath) { 0 };  // the top: the window stays in the resources
    }
    const size_t offset = file_window_basename(folder.value) - folder.value;
    if (offset == strlen(folder.value)) {
        return (FolderPath) { 0 }; // filesystem root
    }
    folder.value[offset] = '\0';
    return file_window_path(folder.value);
}
