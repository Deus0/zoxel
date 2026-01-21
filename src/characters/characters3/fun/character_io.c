void load_character_p(ecs *world, entity realm, entity e, float3 *position, float3 *euler, float4 *rotation) {
    zox_geter(realm, SaveGamePath, path);

    SaveDataCharacter save;
    load2_player(path->value, "player.dat", &save);

    *position = save.position;
    *euler = save.euler;
    *rotation = quaternion_from_euler(save.euler);
}

float3 load_player_position(ecs *world, entity e) {
    zox_geter(e, SaveGamePath, path);
    SaveDataCharacter save;
    load2_player(path->value, "player.dat", &save);
    return save.position;
}

void load_character_e(ecs *world, entity realm, entity e) {
    zox_geter(realm, SaveGamePath, path);

    SaveDataCharacter save;
    load2_player(path->value, "player.dat", &save);

    // TODO: Fix this need to teleport up
    // zox_set(e, Position3D, { float3_add(save.position, (float3) { 0, 0.5f, 0}) });
    zox_set(e, Position3D, { save.position });
    zox_set(e, Euler, { save.euler });
    zox_set(e, Rotation3D, { quaternion_from_euler(save.euler) });
    //zox_set(e, DisableGravity, { 0 });
    //zox_set(e, DisableMovement, { 0 });
}