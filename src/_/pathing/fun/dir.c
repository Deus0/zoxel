byte has_path_directory(const char *path) {
#ifdef zox_windows
    DWORD attrib = GetFileAttributesA(path);
    if (attrib != INVALID_FILE_ATTRIBUTES && (attrib & FILE_ATTRIBUTE_DIRECTORY)) {
        return 1;  // Save game directory exists
    }
#else
    struct stat st;
    if (stat(path, &st) == 0 && S_ISDIR(st.st_mode)) {
        return 1;  // Save game directory exists
    }
#endif
    return 0; // Save game directory does not exist
}



int delete_directory_contents(const char *path) {
    DIR *dir = opendir(path);
    struct dirent *entry;
    char filepath[300];
    int result = 0;
    if (!dir) {
        perror("Error opening directory");
        return -1;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        snprintf(filepath, sizeof(filepath), "%s/%s", path, entry->d_name);

#ifdef _WIN32
        DWORD attrib = GetFileAttributes(filepath);
        if (attrib & FILE_ATTRIBUTE_DIRECTORY) {
            result = delete_directory_contents(filepath);
            if (result != 0) break;
            if (!RemoveDirectory(filepath)) {
                perror("Error deleting directory");
                result = -1;
                break;
            }
        } else {
            if (!DeleteFile(filepath)) {
                perror("Error deleting file");
                result = -1;
                break;
            }
        }
#else
        struct stat st;
        if (stat(filepath, &st) == 0 && S_ISDIR(st.st_mode)) {
            result = delete_directory_contents(filepath);
            if (result != 0) break;
            if (rmdir(filepath) != 0) {
                perror("Error deleting directory");
                result = -1;
                break;
            }
        } else {
            if (remove(filepath) != 0) {
                perror("Error deleting file");
                result = -1;
                break;
            }
        }
#endif
    }
    closedir(dir);
    return result;
}

// 1 success
byte delete_dir(const char* path) {
    if (has_path_directory(path)) {
        zox_logv(" + deleting directory [%s]", path);
        if (delete_directory_contents(path) == 0) {
#ifdef _WIN32
            if (RemoveDirectory(path)) {
                zox_logv(" > directory [%s] deleted", path);
                return 1;
            } else {
                perror("Error deleting directory");
            }
#else
            if (rmdir(path) == 0) {
                zox_logv(" > directory [%s] deleted", path);
                return 1;
            } else {
                perror("Error deleting directory");
            }
#endif
        }
    }
    return 0;
}


// helper: get folder name from a path
// returns malloc'd string (caller must free)
char* get_folder_name(const char *path) {
    if (!path) return NULL;

    size_t len = strlen(path);
    if (len == 0) return NULL;

    // strip trailing slash if exists
    while (len > 0 && (path[len-1] == '/' || path[len-1] == '\\'))
        len--;

    if (len == 0) return NULL;

    // find last slash
    const char *last_slash = path + len - 1;
    while (last_slash > path && *last_slash != '/' && *last_slash != '\\')
        last_slash--;

    if (*last_slash == '/' || *last_slash == '\\')
        last_slash++;

    char *folder = malloc(strlen(last_slash) + 1);
    strcpy(folder, last_slash);
    return folder;
}

