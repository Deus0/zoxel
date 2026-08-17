#ifdef zox_sdl
    #include "wav.c"
#endif

// File implementation for Sounds
void load_files_sounds(ecs *world) {
    byte dbg_log = 0;
    if (nosounds) {
        zox_logv("Sounds are disabled: No Loading Sounds.");
        return;
    }
#ifdef zox_sdl
    zox_logv("Sounds are disabled: No Loading Sounds.");
    return;
#endif
    char* load_directory = concat_file_path(resources_path, directory_sounds);
    zox_logv("  - Loading Files Sounds [%s]", load_directory);
    FileList files = get_files(load_directory, 0);
    sound_files_count = files.count;
    files_sounds = malloc(sizeof(entity) * files.count);
    files_hashmap_sounds = create_string_hashmap(files.count);
    zox_logv(" + io loaded [sounds] [%i]", files.count);
    for (int i = 0; i < files.count; i++) {
#if defined(zox_sdl) // zox_sdl_mixer)
        char* filepath = files.files[i];
        char* filename = files.filenames[i];
        zox_logv("   - [%i] [sound] [%s]", i, filepath);
        uint samples = 0;
        float length = 0;
        float* values = NULL;
        if (!load_wav_data(filepath, &values, &samples, &length, dbg_log)) {
            files_sounds[i] = 0;
        }
        // Spawn our loaded data as sounds
        entity e = spawn_sound_filepath(
            world,
            prefab_sound_filepath,
            values,
            samples,
            length);
        zox_logv("   - [%i] [sound] [%s] - length [%f]", i, filepath, length);
        files_sounds[i] = e;
        string_hashmap_add(files_hashmap_sounds, new_string_data_clone(filename), e);
#else
        files_sounds[i] = 0;
#endif
    }
    free_files(&files);
}

void dispose_files_sounds() {
    if (!files_hashmap_sounds) {
        zox_loge("cannot dispose null files_hashmap_sounds")
    } else {
        zox_logv("Disposing [%i] [sounds]", files_hashmap_sounds->size);
        string_hashmap_dispose(files_hashmap_sounds);
        files_hashmap_sounds = NULL;
        free(files_sounds);
        files_sounds = NULL;
    }
}
