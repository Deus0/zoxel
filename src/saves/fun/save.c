byte save_file_struct(const char *game_path, const char *filename, const void *data, size_t size) {
    char *path = join_path(game_path, filename);
    if (!path) {
        zox_loge("[save_file_struct] failed to build path");
        return 0;
    }
    FILE *file = fopen(path, "wb");
    if (!file) {
        zox_loge("[save_file_struct] failed opening [%s]", path);
        free(path);
        return 0;
    }
    if (fwrite(data, size, 1, file) != 1) {
        zox_loge("[save_file_struct] failed writing [%s]", path);
        fclose(file);
        free(path);
        return 0;
    }
    if (fclose(file) != 0) {
        zox_loge("[save_file_struct] failed closing [%s]", path);
        free(path);
        return 0;
    }
    free(path);
    return 1;
}