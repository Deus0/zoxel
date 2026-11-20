// copies a file, 1 if all of it was written
byte zox_copy_file(const char* from, const char* to) {
    FILE* in = fopen(from, "rb");
    if (!in) {
        return 0;
    }
    FILE* out = fopen(to, "wb");
    if (!out) {
        fclose(in);
        return 0;
    }
    char buffer[65536];
    size_t read;
    byte success = 1;
    while ((read = fread(buffer, 1, sizeof(buffer), in)) > 0) {
        success &= fwrite(buffer, 1, read, out) == read;
    }
    fclose(in);
    fclose(out);
    return success;
}

// copies a folder and all in it (cp -r), 1 if every file was copied; to is made if it isn't there
byte zox_copy_directory(const char* from, const char* to) {
    DIR* dir = opendir(from);
    if (!dir || !zox_mkdirs(to)) {
        if (dir) {
            closedir(dir);
        }
        return 0;
    }
    byte success = 1;
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
            continue;
        }
        char from_path[1024], to_path[1024];
        snprintf(from_path, sizeof(from_path), "%s/%s", from, entry->d_name);
        snprintf(to_path, sizeof(to_path), "%s/%s", to, entry->d_name);
        success &= zox_is_directory(from_path) ? zox_copy_directory(from_path, to_path) : zox_copy_file(from_path, to_path);
    }
    closedir(dir);
    return success;
}

// the running game's own file (zoxel.bin, zoxel.exe), 1 if found
byte zox_executable_path(char* path, int size) {
#ifdef zox_windows
    const DWORD length = GetModuleFileNameA(NULL, path, (DWORD) size);
    return length > 0 && length < (DWORD) size;
#else
    const ssize_t length = readlink("/proc/self/exe", path, size - 1);
    if (length <= 0) {
        return 0;
    }
    path[length] = 0;
    return 1;
#endif
}
