void load_character_p(ecs *world, entity realm, entity e, float3 *position, float3 *euler, float4 *rotation) {
    zox_geter(realm, FolderPath, path);
    SaveDataCharacter save;
    load2_player(path->value, "player.dat", &save);
    *position = save.position;
    *euler = save.euler;
    *rotation = quaternion_from_euler(save.euler);
}

float3 load_character_transform(ecs *world, entity realm, entity e) {
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
}
