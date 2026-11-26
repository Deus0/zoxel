// Immediate entries, sorted with directories first. The caller frees each path and the array.
typedef struct {
    char* path;
    byte directory;
} DirectoryEntry;

static int compare_directory_entries(const void* a, const void* b) {
    const DirectoryEntry* x = a;
    const DirectoryEntry* y = b;
    return x->directory == y->directory ?
        strcmp(x->path, y->path) : y->directory - x->directory;
}

static DirectoryEntry* get_directory_entries(const char* folder, size_t* count) {
    *count = 0;
    DIR* dir = opendir(folder);
    if (!dir) {
        return NULL;
    }
    DirectoryEntry* entries = NULL;
    size_t capacity = 0;
    struct dirent* entry;
    while ((entry = readdir(dir))) {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
            continue;
        }
        if (*count == capacity) {
            const size_t next = capacity ? capacity * 2 : 16;
            DirectoryEntry* grown = realloc(entries, next * sizeof(*entries));
            if (!grown) {
                break;
            }
            entries = grown;
            capacity = next;
        }
        char* path = join_path(folder, entry->d_name);
        if (!path) {
            break;
        }
        struct stat info;
        if (stat(path, &info) || (!S_ISDIR(info.st_mode) && !S_ISREG(info.st_mode))) {
            free(path);
            continue;
        }
        entries[(*count)++] = (DirectoryEntry) { path, S_ISDIR(info.st_mode) };
    }
    closedir(dir);
    if (*count > 1) {
        qsort(entries, *count, sizeof(*entries), compare_directory_entries);
    }
    return entries;
}
