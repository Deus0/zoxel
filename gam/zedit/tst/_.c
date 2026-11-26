// tst: zedit_boot, zedit boots into the editor's hierarchy and inspector (eve/, on zox_game_boot), no main menu
//  - run: zox_tst_game=zedit bash bsh/tst.sh zedit_boot
double zox_tst_zb_time = 0;

byte zox_tst_zedit_boot_ready(ecs* world) {
    zox_tst_waiting = "the hierarchy and inspector";
    const entity canvas = zox_get_link(world, zox_players[0], CanvasLink);
    if (!zox_valid(canvas) ||
        !zox_valid(zox_get_child_by_id(world, canvas, zox_id(HierarchyUI))) ||
        !get_canvas_inspector(world, canvas))
    {
        return 0;
    }
    if (!zox_tst_zb_time) {
        zox_tst_zb_time = zox_current_time;
    }
    return zox_current_time - zox_tst_zb_time > 1;     // a second on: no menu came
}

void zox_tst_zedit_boot_check(ecs* world) {
    const entity player = zox_players[0];
    const entity canvas = zox_get_link(world, player, CanvasLink);
    zox_tst_check(!zox_valid(zox_get_link(world, player, MenuStart)), "a start menu opened");
    zox_tst_check(zox_tst_menus_open(world, canvas) == 0, "[%i] menus open", zox_tst_menus_open(world, canvas));
    zox_tst_check(zox_getv(local_game, LastGameState) == zox_game_boot, "the last game state [%i], not boot",
        zox_getv(local_game, LastGameState));
}

void zox_tsts_zedit(ecs* world) {
    (void) world;
    zox_tst_add("zedit_boot", zox_tst_zedit_boot_ready, zox_tst_zedit_boot_check, 60);
}
