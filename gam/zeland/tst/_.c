// tst: zeland_boot, zeland starts a game (--autostart) and plays: no voxel terrain, no character, a free camera, no errors (the game's ui skips what needs a character)
//  - run: zox_tst_game=zeland bash bsh/tst.sh zeland_boot
byte zox_tst_zeland_boot_ready(ecs* world) {
    (void) world;
    zox_tst_waiting = "the game and its player to play";
    return zox_valid(local_game) && zox_getv(local_game, GameState) == zox_game_state_playing &&
        zox_getv(zox_players[0], PlayerState) == zox_player_state_playing;
}

void zox_tst_zeland_boot_check(ecs* world) {
    const entity player = zox_players[0];
    const entity realm = zox_get_link(world, local_game, RealmLink);
    const entity camera = zox_get_link(world, player, CameraLink);
    zox_tst_check(zox_valid(realm), "no realm");
    zox_tst_check(!zox_valid(zox_get_child_by_id(world, realm, zox_id(Terrain))), "a voxel terrain spawned");
    const entity terrain = zox_get_child_by_id(world, realm, zox_id(PolyTerrain));
    zox_tst_check(zox_valid(terrain) && zox_getv(terrain, MeshRenderCount) > 0, "the realm has no uploaded poly terrain");
    zox_tst_check(!zox_valid(zox_get_link(world, player, CharacterLink)), "a character spawned");
    zox_tst_check(zox_valid(camera) && zox_has(camera, Roaming), "the camera isn't flying free");
    const uint errors = zox_log_errors_count - zox_tst_errors_start;
    zox_tst_check(errors == 0, "[%u] errors logged", errors);
}

void zox_tsts_zeland(ecs* world) {
    (void) world;
    zox_tst_add("zeland_boot", zox_tst_zeland_boot_ready, zox_tst_zeland_boot_check, 60);
}
