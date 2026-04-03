entity spawn_character3_player(ecs *world,spawn_character3D_data spawn_data) {

    entity player = spawn_data.player;

    spawn_data.prefab = prefab_character3_player;
    spawn_data.scale = vox_model_scale;

    entity e = spawn_character3(world, spawn_data);
    // make_test_skeleton(world, e);
    zox_name("character3_player");

    if (spawn_data.terrain) {
        zox_set(e, TerrainLink, { spawn_data.terrain });
    }
    // do this in character spawn code (for character3_player)
    zox_geter_value(player, CameraLink, entity, camera);

    zox_set(e, PlayerLink, { player });
    zox_set(e, CameraLink, { camera });
    zox_set(player, CharacterLink, { e });

    attach_camera_to_character(world, camera, e);

    return e;
}
