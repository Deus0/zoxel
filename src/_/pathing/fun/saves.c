

/*
    User-specific Config Directory: ~/.config/<YourGame>
    User-specific Data Directory: ~/.local/share/<YourGame>
    */

byte get_save_filepath(
    const char *game,
    const char *filename,
    char *path,
    size_t size
) {
    char dir[max_path_characters];
    get_save_directory(game, dir, sizeof(dir));
    int written = snprintf(path, size, "%s%c%s", dir, char_slash, filename);
    if (written < 0 || (size_t) written >= size) {
        fprintf(stderr, "⚠️ get_save_filepath: Path truncated [%s/%s] (%d >= %zu)\n", game, filename, written, size);
        return 0;
    }
    return 1;
}

byte has_save_game_file(const char* game_path, const char* filename) {
    char *path = join_path(game_path, filename);
    // char path[max_path_characters];
    // get_save_filepath(game, filename, path, sizeof(path));
    FILE *file = fopen(path, "rb");
    free(path);
    if (file) {
        fclose(file);
        return 1;
    }
    return 0;
}
