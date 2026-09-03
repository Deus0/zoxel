// #define zox_print_texture_files
// TODO: Load multiple directories
/*void load_files_textures(ecs *world) {
    char* load_directory = concat_file_path(
        resources_path,
        directory_textures);
    zox_logv("  - Loading Files Textures [%s]",
        load_directory);
        FileList files = get_files(load_directory, 0);
        free(load_directory);
    files_count_textures = files.count;
    files_textures = malloc(sizeof(entity) * files.count);
    files_hashmap_textures = create_string_hashmap(files.count);
    zox_logv(" + io loaded [textures] [%i]", files.count);
    for (int i = 0; i < files.count; i++) {
        char* filepath = files.files[i];
        char* filename = files.filenames[i];
        zox_logv("   - [%i] [texture] [%s]", i, filepath);
        entity e = spawn_texture_filepath(
            world,
            prefab_texture,
            filepath);
        if (e) {
            zox_set_unique_name(e, filename);
            string_hashmap_add(files_hashmap_textures, new_string_data_clone(filename), e);
        }
        files_textures[i] = e;
    }
    free_files(&files);
}*/

void dispose_textures_files(ecs *world, void *ctx) {
    zox_logv(" > disposing [%i] [textures]", files_hashmap_textures->size);
    string_hashmap_dispose(files_hashmap_textures);
    free(files_textures);
    files_hashmap_textures = NULL;
    files_textures = NULL;
}

// NOTE: Loads texture files from a resource path.
void load_files_textures_path(
    ecs* world,
    const char* resources_path)
{
    if (!resources_path) {
        return;
    }
    char* load_directory = concat_file_path(
        resources_path,
        directory_textures);
    if (load_directory == NULL) {
        return;
    }
    zox_logv("  - Loading Files Textures [%s]",
        load_directory);
    FileList files = get_files(load_directory, 0);
    free(load_directory);
    if (!files.count) {
        free_files(&files);
        return;
    }
    files_textures = realloc(
        files_textures,
        sizeof(entity) * (files_count_textures + files.count));
    for (int i = 0; i < files.count; i++) {
        char* filepath = files.files[i];
        char* filename = files.filenames[i];
        int index = files_count_textures + i;
        zox_logv( "   - [%i] [texture] [%s]",
            index,
            filepath);
        entity e = spawn_texture_filepath(
            world,
            prefab_texture,
            filepath);
        if (e) {
            zox_set_unique_name(e, filename);
            string_hashmap_add(
                files_hashmap_textures,
                new_string_data_clone(filename),
                e);
        }
        files_textures[index] = e;
    }
    files_count_textures += files.count;
    free_files(&files);
}

// NOTE: Loads texture files from all available resource paths.
void load_files_textures(ecs* world) {
    files_count_textures = 0;
    files_textures = NULL;
    files_hashmap_textures = create_string_hashmap(64);
    load_files_textures_path(world, resources_path);
    load_files_textures_path(world, resources_path_game);
    zox_logv( " + io loaded [textures] [%i]",
        files_count_textures);
}
