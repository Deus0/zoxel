void load_files_voxes(ecs *world) {
    byte dbg_log = 0 || is_verbose;
    entity prefab = prefab_vox; // prefab_vox_file;
    char* load_directory = concat_file_path(resources_path, directory_voxes);
    if (dbg_log) {
        zox_log("Loading Vox Files  [%s]", load_directory);
    }
    FileList files = get_files(load_directory, 0);
    files_voxes_count = files.count;
    files_voxes = malloc(sizeof(entity) * files_voxes_count);
    files_hashmap_voxes = create_string_hashmap(files_voxes_count);
    if (dbg_log) {
        zox_log("   + Count [%i]", files.count);
    }
    for (int i = 0; i < files.count; i++) {
        char* filepath = files.files[i];
        char* filename = files.filenames[i];
        files_voxes[i] = 0;
        if (dbg_log) {
            zox_log("   - [%i] [vox] [%s]", i, filepath);
        }
        vox_file data;
        if (read_vox(filepath, &data) == EXIT_FAILURE) {
            zox_logw("[%s] failed to load", filepath);
            continue;
        }
        if (!data.chunks) {
            zox_logw("[%s] failed to load properly", filepath);
            continue;
        }
        entity e = spawn_vox_file(world, prefab, &data, filename);
        dispose_vox_file(&data);
        if (e) {
            string_hashmap_add(files_hashmap_voxes, new_string_data_clone(filename), e);
            files_voxes[i] = e;
        }
    }
    free_files(&files);
}

void dispose_files_voxes(ecs* world) {
    zox_logv("Disposing [%i] [voxes]", files_hashmap_voxes->size);
    string_hashmap_dispose(files_hashmap_voxes);
    files_hashmap_voxes = NULL;
    free(files_voxes);
    files_voxes = NULL;
}
