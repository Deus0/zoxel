static inline float float_to_precision(const float v, const float precision) {
    return (float) ( (int) (v * precision) ) / precision;
}

zox_sys2(CharacterSaveSystem) {
    float precision_level = 100.0f;    // 100
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RealmLink);
    zox_sys_in(Position3D);
    zox_sys_in(Euler);
    zox_sys_out(SaveHash);
    for (int i = 0; i < it->count; i++) {
        // zox_sys_e();
        zox_sys_i(RealmLink, realm);
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
        if (!zox_valid(realm->value) || !zox_has(realm->value, FolderPath)) {
            zox_logw("Realm Invalid for Saving.");
            continue;
        }
        zox_geter(realm->value, FolderPath, path);
        save2_player(path->value, "player.dat", &data);
        // zox_log("+ new hash detected at [%fx%fx%f] - %lu", position->value.x, position->value.y, position->value.z, hash)
        hash->value = new_hash;
    }
} zox_sys_end(CharacterSaveSystem);
