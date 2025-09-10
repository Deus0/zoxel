void load_character_p(
    ecs *world,
    const entity realm,
    const entity e,
    float3 *position,
    float3 *euler,
    float4 *rotation
) {
    zox_geter(realm, SaveGamePath, path);
    SaveDataCharacter save;
    load2_player(path->value, "player.dat", &save);
    *position = save.position;
    *euler = save.euler;
    *rotation = quaternion_from_euler(save.euler);
}

void load_character_e(
    ecs *world,
    const entity realm,
    const entity e
) {
    zox_geter(realm, SaveGamePath, path);
    SaveDataCharacter save;
    load2_player(path->value, "player.dat", &save);
    zox_set(e, Position3D, { float3_add(save.position, (float3) { 0, 0.1f, 0}) });
    zox_set(e, Euler, { save.euler });
    zox_set(e, Rotation3D, { quaternion_from_euler(save.euler) });
    zox_set(e, DisableGravity, { 0 });
    zox_set(e, DisableMovement, { 0 });
    // zox_log("save.euler: %fx%fx%f", save.euler.x, save.euler.y, save.euler.z)
}