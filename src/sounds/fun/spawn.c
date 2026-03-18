entity spawn_sound_from_file_index(ecs *world, entity p, int index) {

    if (index >= sound_files_count || !files_sounds[index]) {
        zox_log_error("sound is null at [%i]", index);
        return 0;
    }

    return spawn_sound_from_file(world, p, files_sounds[index], 0, get_volume_sfx());
}

entity spawn_sound_from_file_name(ecs *world, entity prefab, char *name, float frequency, float volume) {

    entity sound_file = string_hashmap_get(files_hashmap_sounds, new_string_data(name));

    if (!sound_file) {
        zox_log_error("sound_file not found [%s]", name);
        return 0;
    }

    return spawn_sound_from_file(world, prefab, sound_file, frequency, volume);
}
