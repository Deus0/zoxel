entity spawn_character3_player(ecs *world, entity prefab, entity player, entity realm, entity terrain, lint seed, float3 position, float4 rotation, const char* name) {
    byte render_disabled = 0;
    byte render_distance = 0;
    entity e = spawn_character3(world, prefab, realm, terrain, seed, render_distance, render_disabled, position, rotation, name);
    zox_name("character3_player");
    zox_add(e, DisableMovement);
    // Player
    zox_set(e, PlayerLink, { player });
    zox_set(player, CharacterLink, { e });
    // camera
    entity camera = zox_getv(player, CameraLink);
    zox_set(e, CameraLink, { camera });
    zox_set(camera, CharacterLink, { e });
    zox_set(camera, EntityTarget, { e });
    zox_set(camera, AttachDirty, { zox_dirty_trigger });
    return e;
}
