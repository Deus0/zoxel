// tst: boot_menu, zoxel boots into its start menu (eve/, on zox_game_boot), once, and no other menu
//  - the game went boot, then none (the menu)
//  - run: bash bsh/tst.sh boot_menu
double zox_tst_bm_time = 0;

byte zox_tst_boot_menu_ready(ecs* world) {
    zox_tst_waiting = "the start menu";
    const entity player = zox_players[0];
    if (!zox_valid(player) || !zox_valid(zox_get_link(world, player, MenuStart))) {
        return 0;
    }
    if (!zox_tst_bm_time) {
        zox_tst_bm_time = zox_current_time;
    }
    return zox_current_time - zox_tst_bm_time > 1;     // a second on: no second menu came
}

void zox_tst_boot_menu_check(ecs* world) {
    const entity player = zox_players[0];
    const entity canvas = zox_get_link(world, player, CanvasLink);
    int starts = 0;
    iter it2 = zox_children(world, canvas);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            starts += zox_has(it2.entities[j], MenuStart);
        }
    }
    zox_tst_check(starts == 1, "[%i] start menus, expected 1", starts);
    zox_tst_check(zox_tst_menus_open(world, canvas) == 0, "[%i] other menus open", zox_tst_menus_open(world, canvas));
    zox_tst_check(zox_getv(local_game, GameState) == zox_game_none, "game state [%i], not none",
        zox_getv(local_game, GameState));
    zox_tst_check(zox_getv(local_game, LastGameState) == zox_game_boot, "the last game state [%i], not boot",
        zox_getv(local_game, LastGameState));
}

void zox_tsts_zoxel(ecs* world) {
    (void) world;
    zox_tst_add("boot_menu", zox_tst_boot_menu_ready, zox_tst_boot_menu_check, 60);
    zox_tst_set_setup("boot_menu", NULL, 1);
}
