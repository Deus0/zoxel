// tst: zedit_boot, zedit boots into the editor's menubar, hierarchy, inspector and files (eve/, on zox_game_boot), no main menu
//  - run: zox_tst_game=zedit bash bsh/tst.sh zedit_boot
double zox_tst_zb_time = 0;

byte zox_tst_zedit_boot_ready(ecs* world) {
    zox_tst_waiting = "the menubar, hierarchy, inspector and files";
    const entity canvas = zox_get_link(world, zox_players[0], CanvasLink);
    if (!zox_valid(canvas) ||
        !zox_get_child_by_id(world, canvas, zox_id(Menubar)) ||
        !zox_valid(zox_get_child_by_id(world, canvas, zox_id(HierarchyUI))) ||
        !get_canvas_inspector(world, canvas) ||
        !zox_get_child_by_id(world, canvas, zox_id(FileWindowUI)))
    {
        return 0;
    }
    zox_tst_waiting = "the game to finish booting";
    if (zox_getv(local_game, GameState) == zox_game_boot) {
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
    const entity4 docks = docks_find(world, canvas);
    zox_tst_check(docks.w && zox_has(docks.w, Menubar), "the menubar did not boot docked");
    zox_tst_check(docks.x && docks.x == zox_get_child_by_id(world, canvas, zox_id(HierarchyUI)) &&
        docks.y && docks.y == get_canvas_inspector(world, canvas) &&
        docks.z && docks.z == zox_get_child_by_id(world, canvas, zox_id(FileWindowUI)), "editor windows did not boot docked");
    if (!zox_disable_post_processing && docks.x && docks.y && docks.z) {
        const entity texture = zox_get_child_by_id(world, canvas, zox_id(RenderTextureScreen));
        const int2 canvas_size = zox_getv(canvas, LayoutSize);
        const int2 origin = { zox_getv(docks.x, LayoutSize).x, zox_getv(docks.z, LayoutSize).y };
        const int2 size = { canvas_size.x - origin.x - zox_getv(docks.y, LayoutSize).x,
                            canvas_size.y - origin.y - zox_getv(docks.w, LayoutSize).y };
        const int2 position = int2_sub(int2_add(origin, int2_half(size)), int2_half(canvas_size));
        zox_tst_check(texture && int2_equals(zox_getv(texture, LayoutSize), size) &&
            int2_equals(zox_getv(texture, LayoutPosition), position), "game texture did not fill the docked viewport");
    }
    zox_tst_check(!zox_valid(zox_get_link(world, player, MenuStart)), "a start menu opened");
    zox_tst_check(zox_tst_menus_open(world, canvas) == 0, "[%i] menus open", zox_tst_menus_open(world, canvas));
    zox_tst_check(zox_getv(local_game, LastGameState) == zox_game_boot, "the last game state [%i], not boot",
        zox_getv(local_game, LastGameState));
}

void zox_tsts_zedit(ecs* world) {
    (void) world;
    zox_tst_add("zedit_boot", zox_tst_zedit_boot_ready, zox_tst_zedit_boot_check, 60);
}
