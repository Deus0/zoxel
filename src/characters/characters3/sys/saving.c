static inline float float_to_precision(const float v, const float precision) {
    return (float) ( (int) (v * precision) ) / precision;
}

/*byte save_player_character(const char *game_path, const char *filename, SaveDataCharacter* data) {
    char * path = join_path(game_path, filename);
    if (!path) {
        zox_loge("[save_player_character] failed to build path");
        return 0;
    }
    FILE* file = fopen(path, "wb");
    if (file == NULL) {
        zox_log_error("[save2] error saving [%s]", path)
        perror("Error opening file for writing");
        free(path);
        return 0;
    }
    size_t written = fwrite(data, sizeof(SaveDataCharacter), 1, file);
    if (written != 1) {
        zox_loge("[save_player_character] failed writing save file [%s]", path);
        perror("fwrite");
        fclose(file);
        free(path);
        return 0;
    }
    if (fclose(file) != 0) {
        zox_loge("[save_player_character] failed closing file [%s]", path);
        perror("fclose");
        free(path);
        return 0;
    }
    free(path);
    return 1;
}*/

// NOTE: Saves Character to file!
zox_sys2(CharacterSaveSystem) {
    float precision_level = 100.0f;    // 100
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Position3D);
    zox_sys_in(Euler);
    zox_sys_out(SaveHash);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Position3D, position);
        zox_sys_i(Euler, euler);
        zox_sys_o(SaveHash, hash);
        // character
        SaveDataCharacter data = {
            .position = position->value,
            .euler = euler->value,
        };
        data.position.x = float_to_precision(data.position.x, precision_level);
        data.position.y = float_to_precision(data.position.y, precision_level);
        data.position.z = float_to_precision(data.position.z, precision_level);
        data.euler.x = float_to_precision(data.euler.x, precision_level);
        data.euler.y = float_to_precision(data.euler.y, precision_level);
        data.euler.z = float_to_precision(data.euler.z, precision_level);
        // check hash
        ulong new_hash = compute_character_hash(&data);
        if (new_hash == hash->value) {
            continue;
        }
        entity realm = zox_get_link(world, e, RealmLink);
        if (!zox_valid(realm) || !zox_has(realm, FolderPath)) {
            zox_logw("Realm Invalid for Saving.");
            continue;
        }
        zox_geter(realm, FolderPath, path);
        if (save_file_struct(path->value, "player.dat", &data, sizeof(SaveDataCharacter))) {
            hash->value = new_hash;
        }
        // zox_log("+ new hash detected at [%fx%fx%f] - %lu", position->value.x, position->value.y, position->value.z, hash)
    }
} zox_sys_end(CharacterSaveSystem);
