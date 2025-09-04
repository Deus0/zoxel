#ifdef _WIN32

int is_symlink(const char *path) {
    DWORD attrs = GetFileAttributesA(path);
    if (attrs == INVALID_FILE_ATTRIBUTES) return 0;
    return (attrs & FILE_ATTRIBUTE_REPARSE_POINT) != 0;
}

#else

#define is_symlink(path) S_ISLNK(statbuf.st_mode)

#endif