void load_player_e(ecs *world, const entity player) {
    zox_geter_value(player, GameLink, entity, game);
    zox_geter_value(game, RealmLink, entity, realm);
    zox_geter_value(player, CharacterLink, entity, character);
    zox_geter_value(player, CameraLink, entity, camera);
    load_character_e(world, realm, character);
    // load_camera_e(world, realm, camera);
}
