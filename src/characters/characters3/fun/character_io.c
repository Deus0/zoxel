/*byte load_player_character_path(const char *game_path, const char *filename, SaveDataCharacter *data) {
    char *path = join_path(game_path, filename);
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        zox_loge("[load_player_character_path] Error opening file [%s]", path);
        free(path);
        return 0;
    }
    // Check file size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);
    if (size != sizeof(SaveDataCharacter)) {
        zox_loge("[load_player_character_path] Invalid save file size (%ld != %zu): %s", size, sizeof(SaveDataCharacter), path);
        fclose(file);
        free(path);
        return 0;
    }
    if (fread(data, sizeof(*data), 1, file) != 1) {
        zox_loge("[load_player_character_path] Failed reading save: %s", path);
        fclose(file);
        free(path);
        return 0;
    }
    fclose(file);
    zox_logv("Loaded from [%s]", path);
    free(path);
    return 1;
}*/

void load_character_player(ecs *world, entity realm, entity e, float3* position, float3* euler, float4* rotation) {
    zox_geter(realm, FolderPath, path);
    SaveDataCharacter save;
    if (load_file_struct(path->value, "player.dat", &save, sizeof(SaveDataCharacter))) {
        *position = save.position;
        *euler = save.euler;
    } else {
        zox_logw("Character Save File Corrupted [%s]", path);
        // TODO: Handle more gracefully
        *position = (float3) { 1, 42, 1 };
        *euler = float3_zero;
    }
    *rotation = quaternion_from_euler(*euler);
}

/*float3 load_character_transform(ecs *world, entity realm, entity e) {
    if (!zox_valid(realm) || !zox_valid(e)) {
        return float3_zero;
    }
    zox_geter(realm, FolderPath, path);
    SaveDataCharacter save;
    load2_player(path->value, "player.dat", &save);
    float3 position = save.position;
    // TODO: adjust this bt floating point error
    // Will saving different timing help??
    position = float3_add(position, (float3) { 0, 0.001f, 0 });
    zox_set(e, Position3D, { position });
    zox_set(e, LastPosition3D, { position });
    zox_set(e, Euler, { save.euler });
    zox_set(e, Rotation3D, { quaternion_from_euler(save.euler) });
    return position;
}

float3 load_player_position(ecs *world, entity e) {
    zox_geter(e, SaveGamePath, path);
    SaveDataCharacter save;
    load2_player(path->value, "player.dat", &save);
    return save.position;
}*/
