entity spawn_character3_player(
    ecs *world,
    entity prefab,
    entity player,
    entity realm,
    entity terrain,
    lint seed,
    float3 position,
    float4 rotation,
    const char* name)
{
    entity camera = zox_get_link(world, player, Camera);
    byte render_disabled = 0;
    byte render_distance = 0;
    entity e = spawn_character3(
        world,
        prefab,
        realm,
        terrain,
        seed,
        render_distance,
        render_disabled,
        position,
        rotation,
        name);
    zox_name("character3_player");
    zox_add(e, DisableMovement);
    // Player
    zox_setv(e, PlayerLink, player);
    zox_setv(player, CharacterLink, e);
    // camera
    zox_link(world, e, Camera, camera);
    zox_setv(camera, CharacterLink, e);
    zox_setv(camera, EntityTarget, e);
    zox_setv(camera, AttachDirty, zox_dirty_trigger);
    // New links
    zox_link(world, e, Player, player);
    zox_link(world, player,  Character, e);
    zox_link(world, e,  Camera, camera);
    zox_link(world, camera,  Character, e);
    return e;
}
