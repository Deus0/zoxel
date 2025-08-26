byte get_save_games(const char *game, char ***dirs, byte* count) {
    char path[max_path_characters];
    get_save_directory(game, path, sizeof(path));
    return get_dirs(path, dirs, count);
}