
#ifdef zox_windows
    #define zox_strdup _strdup
#else
    #define zox_strdup strdup
#endif


// --- internal helpers -------------------------------------------------------

char* join_path(const char *base, const char *name) {
    if (!base || !name) return NULL;
    size_t bl = strlen(base), nl = strlen(name);
    int need_sep = (bl > 0 && base[bl-1] != '/' && base[bl-1] != '\\');
    size_t total = bl + (need_sep ? 1 : 0) + nl + 1;

    char *out = (char*)malloc(total);
    if (!out) return NULL;

    memcpy(out, base, bl);
    size_t i = bl;
    if (need_sep) out[i++] = char_slash;
    memcpy(out + i, name, nl);
    out[i + nl] = '\0';
    return out;
}

#ifndef zox_windows
int is_dir_posix(const char *fullpath) {
    struct stat st;
    if (stat(fullpath, &st) != 0) return 0;
    return S_ISDIR(st.st_mode) ? 1 : 0;
}
#endif

int push_string(char ***arr, byte *count, const char *s) {
    if (!arr || !count || !s) return -1;
    // prevent overflow of byte
    if (*count == 0xFF) return -2;

    char **tmp = (char**)realloc(*arr, (size_t)(*count + 1) * sizeof(char*));
    if (!tmp) return -3;
    *arr = tmp;

    (*arr)[*count] = zox_strdup(s);
    if (!(*arr)[*count]) return -3;

    (*count)++;
    return 0;
}

// --- API --------------------------------------------------------------------

/**
 * get_dirs
 *   - path: base directory to scan
 *   - out_list: array of FULL PATH strings to immediate subdirectories
 *   - out_count: number of entries (byte)
 * Returns 0 on success (including empty), <0 on error.
 *
 * Caller frees each string then the list.
 */
int get_dirs(const char *path, char ***out_list, byte *out_count) {
    if (!path || !out_list || !out_count) return -1;

    *out_list = NULL;
    *out_count = 0;

#ifdef zox_windows
    char search_path[MAX_PATH];
    // Build search pattern: <path>\*
    // Handle trailing separator
    size_t pl = strlen(path);
    if (pl + 3 >= sizeof(search_path)) return -1;
    if (pl > 0 && (path[pl-1] == '\\' || path[pl-1] == '/')) {
        snprintf(search_path, sizeof(search_path), "%s*", path);
    } else {
        snprintf(search_path, sizeof(search_path), "%s\\*", path);
    }

    WIN32_FIND_DATA fd;
    HANDLE h = FindFirstFile(search_path, &fd);
    if (h == INVALID_HANDLE_VALUE) return -1;

    do {
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (strcmp(fd.cFileName, ".") == 0 || strcmp(fd.cFileName, "..") == 0) continue;

            char *full = join_path(path, fd.cFileName);
            if (!full) { FindClose(h); return -3; }

            int r = push_string(out_list, out_count, full);
            free(full);
            if (r < 0) { FindClose(h); return r; }
        }
    } while (FindNextFile(h, &fd));
    FindClose(h);

#else
    DIR *dir = opendir(path);
    if (!dir) return -1;

    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL) {
        const char *name = ent->d_name;
        if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) continue;

        // Never trust d_type; stat to confirm directory.
        char *full = join_path(path, name);
        if (!full) { closedir(dir); return -3; }

        if (is_dir_posix(full)) {
            int r = push_string(out_list, out_count, full);
            free(full);
            if (r < 0) { closedir(dir); return r; }
        } else {
            free(full);
        }
    }
    closedir(dir);
#endif

    return 0;
}

void free_dirs(char **list, byte count) {
    if (!list) return;
    for (byte i = 0; i < count; i++) free(list[i]);
    free(list);
}