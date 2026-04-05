entity spawn_character3_player(ecs *world,spawn_character3D_data data) {

    entity player = data.player;

    data.prefab = prefab_character3_player;
    data.scale = vox_model_scale;

    entity e = spawn_character3(world, data);
    zox_name("character3_player");

    zox_set(e, PlayerLink, { player });
    zox_set(player, CharacterLink, { e });

    if (data.terrain) {
        zox_set(e, TerrainLink, { data.terrain });
    }

    // TODO: Attach in seperate system
    zox_geter_value(player, CameraLink, entity, camera);

    zox_set(e, CameraLink, { camera });
    zox_set(camera, CharacterLink, { e });

    zox_set(camera, EntityTarget, { e });
    zox_set(camera, AttachDirty, { zox_dirty_trigger });

    return e;
}
