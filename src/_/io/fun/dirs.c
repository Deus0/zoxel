// is there a folder at path
byte zox_is_directory(const char* path) {
    struct stat info;
    return stat(path, &info) == 0 && S_ISDIR(info.st_mode);
}

// makes a folder (its parent must be there), 1 if it's there after: made, or already was
//  - posix folders are 0755 (the owner writes, others read)
byte zox_mkdir(const char* path) {
#ifdef zox_windows
    const int made = mkdir(path);
#else
    const int made = mkdir(path, 0755);
#endif
    return made == 0 || (errno == EEXIST && zox_is_directory(path));
}

// makes a folder and each folder on the way (mkdir -p), 1 if it's there after
//  - splits at / and \, skips a windows drive (C:)
byte zox_mkdirs(const char* path) {
    char partial[1024];
    const size_t length = strlen(path);
    if (!length || length >= sizeof(partial)) {
        return 0;
    }
    memcpy(partial, path, length + 1);
    for (size_t i = 1; i < length; i++) {
        if (partial[i] != '/' && partial[i] != '\\') {
            continue;
        }
        if (partial[i - 1] == ':' || partial[i - 1] == '/' || partial[i - 1] == '\\') {
            continue;
        }
        const char kept = partial[i];
        partial[i] = 0;
        const byte made = zox_mkdir(partial);
        partial[i] = kept;
        if (!made) {
            return 0;
        }
    }
    return zox_mkdir(partial);
}
