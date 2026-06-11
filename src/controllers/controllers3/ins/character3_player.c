entity spawn_character3_player(ecs *world, entity prefab, entity realm, entity terrain, lint seed, entity model, byte render_depth, byte render_disabled, float3 position, float4 rotation, const char* name, entity player) {
    zox_geter_value(player, CameraLink, entity, camera);
    entity e = spawn_character3(world, prefab, realm, terrain, seed, model, render_depth, render_disabled, position, rotation, name);
    zox_name("character3_player");
    zox_set(e, PlayerLink, { player });
    zox_set(player, CharacterLink, { e });
    zox_set(e, CameraLink, { camera });
    zox_set(camera, CharacterLink, { e });
    zox_set(camera, EntityTarget, { e });
    zox_set(camera, AttachDirty, { zox_dirty_trigger });
    return e;
}
