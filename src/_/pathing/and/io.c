byte create_directory(const char* path) {
    if (mkdir(path, 0777) != 0) {
        zox_loge("directory failed to create [%s]", path);
        return 0;
    } else {
        return 1;   // success
    }
}

int directory_exists(const char *path) {
    struct stat info;
    if (stat(path, &info) != 0) {
        // Can't access path (doesn't exist or other error)
        return 0;
    } else {
        // Check if it is a directory
        return (info.st_mode & S_IFDIR) != 0;
    }
}

byte android_create_directory_r(const char* path) {
    if (directory_exists(path)) {
        return 1;
    }
    char parent[1024];
    strcpy(parent, path);
    // remove last directory component from parent
    char* last_slash = strrchr(parent, '/');
    if (last_slash) {
        *last_slash = 0;
        if (!android_create_directory_r(parent)) {
            return 0;
        }
    }
    return mkdir(path, 0777) == 0 || errno == EEXIST;
}

void delete_directory_recursive(const char* path) {
    DIR* dir = opendir(path);
    if (dir != NULL) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
            char* sub_path = (char*) malloc(strlen(path) + strlen(entry->d_name) + 2);
            sprintf(sub_path, "%s/%s", path, entry->d_name);
            if (entry->d_type == DT_DIR) delete_directory_recursive(sub_path);
            else remove(sub_path);
            free(sub_path);
        }
        closedir(dir);
        rmdir(path);
    } else {
        zox_log("Error opening directory %s: %s\n", path, strerror(errno))
    }
}