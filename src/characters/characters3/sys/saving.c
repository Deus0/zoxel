static inline float float_to_precision(const float v, const float precision) {
    return (float) ( (int) (v * precision) ) / precision;
}

zox_sys2(CharacterSaveSystem) {
#ifdef zox_disable_save_games
    return;
#endif

    float precision_level = 100.0f;    // 100
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RealmLink);
    zox_sys_in(Position3D);
    zox_sys_in(Euler);
    zox_sys_out(CharacterSaveHash);
    for (int i = 0; i < it->count; i++) {
        // zox_sys_e();
        zox_sys_i(RealmLink, realm);
        zox_sys_i(Position3D, position);
        zox_sys_i(Euler, euler);
        zox_sys_o(CharacterSaveHash, characterSaveHash);

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
        ulong hash = compute_character_hash(&data);
        if (hash == characterSaveHash->value) {
            continue;
        }

        /*zox_geter_value(e, TerrainLink, entity, terrain);
        if (!terrain) zox_log_error("terrain link issue");
        if (!terrain) continue;
        zox_geter_value(terrain, RealmLink, entity, realm);
        if (!terrain) zox_log_error("realm link issue");
        if (!realm) continue;*/
        if (!zox_valid(realm->value) || !zox_has(realm->value, SaveGamePath)) {
            zox_logw("Realm Invalid for Saving.");
            continue;
        }
        zox_geter(realm->value, SaveGamePath, path);


        save2_player(path->value, "player.dat", &data);
        // save camera - move this to camera save system
        /*zox_geter_value(e, CameraLink, entity, camera);
        if (zox_valid(camera)) {
            SaveDataCamera data2 = {
                .position = zox_gett_value(camera, Position3D),
                .euler = zox_gett_value(camera, Euler),
                .lrotation = zox_gett_value(camera, LocalRotation3D),
            };
            save2_camera(path->value, "camera.dat", &data2);
        }*/
        // zox_log("+ new hash detected at [%fx%fx%f] - %lu", position->value.x, position->value.y, position->value.z, hash)
        characterSaveHash->value = hash;
    }
} zox_sys_end(CharacterSaveSystem);
