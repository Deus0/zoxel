// folders are made with zox_mkdir and zox_mkdirs (_/io)
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