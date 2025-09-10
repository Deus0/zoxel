static inline float float_to_precision(const float v, const float precision) {
    return (float) ( (int) (v * precision) ) / precision;
}

void CharacterSaveSystem(iter *it) {
#ifdef zox_disable_save_games
    return;
#endif
    const float precision_level = 10.0f;    // 100

    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Position3D);
    zox_sys_in(Euler);
    zox_sys_out(CharacterSaveHash);

    for (int i = 0; i < it->count; i++) {

        zox_sys_e();
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

        zox_geter_value(e, TerrainLink, entity, terrain);
        if (!terrain) zox_log_error("terrain link issue");
        if (!terrain) continue;
        zox_geter_value(terrain, RealmLink, entity, realm);
        if (!terrain) zox_log_error("realm link issue");
        if (!realm) continue;
        zox_geter(realm, SaveGamePath, path);


        save2_player(path->value, "player.dat", &data);
        // save camera - move this to camera save system
        zox_geter_value(e, CameraLink, ecs_entity_t, camera);
        SaveDataCamera data2 = {
            .camera_euler = zox_gett_value(camera, Euler),
            .camera_rotation_local = zox_gett_value(camera, LocalRotation3D),
        };
        save2_camera(path->value, "camera.dat", &data2);
        // zox_log("+ new hash detected at [%fx%fx%f] - %lu", position->value.x, position->value.y, position->value.z, hash)
        characterSaveHash->value = hash;
    }
} zoxd_system(CharacterSaveSystem)