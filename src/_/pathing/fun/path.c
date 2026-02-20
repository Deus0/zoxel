void debug_base_path(const char *base_path) {
    DIR *dir;
    struct dirent *entry;
    if ((dir = opendir(base_path)) != NULL) {
        zox_log(" > directories and files in [%s]\n", base_path)
        while ((entry = readdir(dir)) != NULL) zox_log("     + [%s]\n", entry->d_name)
        closedir(dir);
    } else zox_log(" - failed to open directory [%s]\n", base_path)
}

char* concat_file_path(const char* path1, const char* path2) {
    if (!path1 || !path2) return NULL;

    size_t len1 = strlen(path1);
    size_t len2 = strlen(path2);

    // handle empty paths safely
    int need_slash = 0;
    if (len1 == 0 || path1[len1 - 1] != char_slash) {
        need_slash = 1;
    }

    size_t total = len1 + need_slash + len2 + 1; // +1 for '\0'

    char* output = malloc(total);
    if (!output) return NULL;

    strcpy(output, path1);

    if (need_slash) {
        strcat(output, character_slash);
    }

    strcat(output, path2);

    return output;
}

char* get_full_file_path(const char* filepath) {
    char* fullpath = malloc(strlen(data_path) + strlen(filepath) + 1);
    strcpy(fullpath, data_path);
    strcat(fullpath, filepath);
#ifdef zoxel_debug_pathing
    zox_log("fullpath: %s\n", fullpath)
#endif
    return fullpath;
}

void convert_file_path_slashes(char* path) {
    while (*path) {
        if (*path == '\\') {
            *path = '/';
        }
        ++path;
    }
}

char* clone_str(const char* text) {
    int length = strlen(text);
    char *new_text = malloc(length);
    memcpy(new_text, text, length);
    return new_text;
}

char* initialize_base_path() {
#ifdef zox_windows
    char* base_path = get_base_path_windows();
#else
    char* base_path = get_base_path_native();
#endif
#ifdef zoxel_debug_base_path
    debug_base_path(base_path);
#endif
    return base_path;
}

char* join_paths(const char* base, const char* folder) {
    size_t len = strlen(base) + strlen(folder) + 2; // +1 for '/' +1 for '\0'
    char* path = malloc(len);
    if (!path) {
        return NULL;
    }
    strcpy(path, base);
    // Add trailing slash if needed
    if (path[strlen(path)-1] != char_slash) {
        strcat(path, character_slash);
    }
    strcat(path, folder);
    return path;
}

int path_exists(const char* path) {
    struct stat st;
    return stat(path, &st) == 0;
}

char* find_resources_path(char* base_path, const char* resources) {
    char* path = NULL;
    char* slash_pos;
    while (1) {
        path = join_paths(base_path, resources);
        if (!path) return NULL;

        if (path_exists(path)) {
            return path; // found it
        }
        free(path);

        // If we’re at root or empty path, stop searching
        if (strcmp(base_path, character_slash) == 0 || strlen(base_path) == 0) {
            return NULL; // Not found
        }

        // Strip last directory from base_path (in place)
        slash_pos = strrchr(base_path, char_slash);
        if (!slash_pos) {
            // No slash found, cannot go up further
            return NULL;
        }
        if (slash_pos == base_path) {
            // Root dir
            base_path[1] = '\0'; // keep "/"
        } else {
            *slash_pos = '\0'; // cut off last directory
        }
    }
}

// sets base_path, data_path
byte initialize_pathing_native(const char* game_name) {
    char* base_path = initialize_base_path();
    if (base_path == NULL) {
        zox_log_error("failed at [initialize_base_path] char_slash [%c]", char_slash);
        return EXIT_FAILURE;
    }
    data_path = base_path;
    zox_logv("Base Path [%s]", base_path);
    DIR* base_dir = opendir(base_path);
    if (base_dir) {
        closedir(base_dir);

        char* check_path = find_resources_path(base_path, resources_folder_name);
        // zox_log("Checking Binary Path [%s] for [res] - check_path [%s]", base_path, check_path);

        if (check_path) {
            DIR* check_dir = opendir(check_path);
            if (check_dir) {
                closedir(check_dir);
                resources_path = check_path;
                zox_logv("[Dev] Resources Path [%s]",   resources_path);
                return EXIT_SUCCESS;
            } else {
                zox_logv("[res] not found locally");
                free(check_path);
            }
        }

        char* check_path2 = malloc(256);
        if (!check_path2) {
            zox_log_error("Malloc failed in [initialize_pathing_native]");
            return EXIT_FAILURE;
        }
        snprintf(check_path2, 256, "/usr/share/%s/res", game_name);

        DIR* resources_dir = opendir(check_path2);
        if (resources_dir) {
            closedir(resources_dir);
            resources_path = check_path2;
            zox_logi("[Live] Resources Path [%s]", resources_path);
            return EXIT_SUCCESS;
        } else {
            zox_log_error("Resources Path did not exist [%s]", check_path2);
            free(check_path2);  // not found, free memory
        }

        zox_log_error("Resources Path cannot open.");
        return EXIT_FAILURE;

    } else if (ENOENT == errno) {
        zox_log("SDL data_path (DOES NOT EXIST): %s\n", data_path);
    } else {
        zox_log("SDL data_path (MYSTERIOUSLY DOES NOT EXIST): %s\n", data_path);
    }
    return EXIT_SUCCESS;
}
