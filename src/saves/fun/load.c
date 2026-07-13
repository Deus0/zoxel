byte load_file_struct(const char *game_path, const char *filename, void *data, size_t size) {
    char *path = join_path(game_path, filename);
    if (!path) {
        zox_loge("[load_file_struct] failed to build path");
        return 0;
    }
    FILE *file = fopen(path, "rb");
    if (!file) {
        zox_loge("[load_file_struct] failed opening [%s]", path);
        free(path);
        return 0;
    }
    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        free(path);
        return 0;
    }
    long file_size = ftell(file);
    if (file_size < 0) {
        fclose(file);
        free(path);
        return 0;
    }
    rewind(file);
    if ((size_t)file_size != size) {
        zox_loge("[load_file_struct] invalid file size (%ld != %zu): %s",
            file_size, size, path);
        fclose(file);
        free(path);
        return 0;
    }
    if (fread(data, size, 1, file) != 1) {
        zox_loge("[load_file_struct] failed reading [%s]", path);
        fclose(file);
        free(path);
        return 0;
    }
    fclose(file);
    free(path);
    return 1;
}