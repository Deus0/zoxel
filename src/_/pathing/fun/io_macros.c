/*#define create_load_and_save_functions(T, name)\
\
void save_##name(const char *game, const char *filename, T *data) {\
    char path[io_path_size];\
    get_save_filepath(game, filename, path, sizeof(path));\
    FILE *file = fopen(path, "wb");\
    if (file == NULL) {\
        zox_log(" > error saving [%s]\n", path)\
        perror("Error opening file for writing");\
        return;\
    }\
    fwrite(data, sizeof(T), 1, file);\
    fclose(file);\
}\
\
byte load_##name(const char *game, const char *filename, T *data) {\
    char path[io_path_size];\
    get_save_filepath(game, filename, path, sizeof(path));\
    FILE *file = fopen(path, "rb");\
    if (file == NULL) {\
        zox_log_error("Error opening file [%s] for reading", path);\
        return 0;\
    }\
    size_t filesize = fread(data, sizeof(T), 1, file);\
    fclose(file);\
    zox_logv("Loaded File from [%s]", path);\
    return filesize > 0;\
}\
\
void save2_##name(const char *game_path, const char *filename, T *data) {\
    char *path = join_path(game_path, filename);\
    FILE *file = fopen(path, "wb");\
    if (file == NULL) {\
        zox_log_error("[save2] error saving [%s]", path)\
        perror("Error opening file for writing");\
        free(path); \
        return;\
    }\
    fwrite(data, sizeof(T), 1, file);\
    fclose(file);\
    free(path); \
}\
\
byte load2_##name(const char *game_path, const char *filename, T *data) {\
    char *path = join_path(game_path, filename);\
    FILE *file = fopen(path, "rb");\
    if (file == NULL) {\
        zox_log_error("[load2] Error opening file [%s]", path);\
        free(path); \
        return 0;\
    }\
    size_t filesize = fread(data, sizeof(T), 1, file);\
    fclose(file);\
    zox_logv("Loaded from [%s]", path);\
    free(path); \
    return filesize > 0;\
}*/
