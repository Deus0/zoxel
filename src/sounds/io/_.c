// File implementation for Sounds
void load_files_sounds(ecs *world) {
    if (nosounds) {
        zox_logv("Sounds are disabled: No Loading Sounds.");
        return;
    }
    char* load_directory = concat_file_path(resources_path, directory_sounds);
    zox_logv("  - Loading Files Sounds [%s]", load_directory);
    FileList files = get_files(load_directory, 0);
    sound_files_count = files.count;
    files_sounds = malloc(sizeof(entity) * files.count);
    files_hashmap_sounds = create_string_hashmap(files.count);
    zox_logv(" + io loaded [sounds] [%i]", files.count);
    for (int i = 0; i < files.count; i++) {
#if defined(zox_sdl_mixer) && !defined(zox_sdl3)
        char* filepath = files.files[i];
        char* filename = files.filenames[i];
        zox_logv("   - [%i] [sound] [%s]", i, filepath);
        Mix_Chunk *mix_chunk = Mix_LoadWAV(filepath);
        if (!mix_chunk) {
            zox_loge("sound file failed to load [%s] due to [%s]", filepath, Mix_GetError());
            files_sounds[i] = 0;
            continue;
        }
        float sound_length = get_mix_chunk_sound_length(mix_chunk);
        float* value = zox_mix_chunk_samples(mix_chunk);
        int length = zox_mix_chunk_length(mix_chunk);
        entity e = spawn_sound_filepath(world, prefab_sound_filepath, value, length, sound_length);
        zox_logv("   - [%i] [sound] [%s] - length [%f]", i, filepath, sound_length);
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
