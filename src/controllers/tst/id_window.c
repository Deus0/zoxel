// tst: id_window, the player's id window: who they are a line each, their name first (stats/ui/ins/id.c)
//  - unmade data reads none, a linked race reads its name (RaceLink)
//  - its mug shot, a render frame of its head (stats/ui/ins/mugshot.c)
//  - saves tst/id_window.ppm to look at
//  - run: bash bsh/tst.sh id_window
byte zox_tst_id_stage = 0;
double zox_tst_id_time = 0;
entity zox_tst_id_window = 0;
char zox_tst_id_before[256];
char zox_tst_id_after[256];

byte zox_tst_id_window_ready(ecs* world) {
    if (!zox_tst_id_stage) {
        if (!zox_tst_player_spawn_ready(world)) {
            return 0;
        }
        const entity character = zox_get_link(world, dbg_player, CharacterLink);
        character_id_text(world, character, zox_tst_id_before, sizeof(zox_tst_id_before));
        zox_tst_id_window = spawn_player_menu_id(world, dbg_player);
        zox_tst_id_stage = 1;
        zox_tst_id_time = zox_current_time;
        return 0;
    }
    zox_tst_progress(zox_tst_id_stage);
    zox_tst_waiting = "the id window";
    if (zox_tst_id_stage == 1) {
        if (zox_current_time - zox_tst_id_time < 1.5) {
            return 0;
        }
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("id_window.ppm"));
        zox_tst_id_stage = 2;
        return 0;
    }
    if (zox_tst_id_stage == 2) {
        if (zox_screenshot_request[0]) {
            return 0;
        }
        // a race made up for it, linked
        const entity character = zox_get_link(world, dbg_player, CharacterLink);
        const entity race = zox_new();
        set_ZoxName(world, race, "Elf");
        zox_link(world, character, RaceLink, race);
        zox_tst_id_stage = 3;
        return 0;
    }
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    character_id_text(world, character, zox_tst_id_after, sizeof(zox_tst_id_after));
    return 1;
}

void zox_tst_id_window_check(ecs* world) {
    zox_log("[tst] id before [%s]", zox_tst_id_before);
    zox_tst_check(zox_valid(zox_tst_id_window) && zox_has(zox_tst_id_window, MenuId), "no id window");
    // its mug shot, a render frame on its body
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    const entity skeleton = get_skeleton(world, character);
    zox_tst_check(zox_valid(skeleton) && zox_valid(zox_tst_id_window) &&
        zox_valid(zox_get_child_by_id_recursive(world, zox_tst_id_window, zox_id(RenderTexture))), "the id window has no mug shot");
    zox_tst_check(strstr(zox_tst_id_before, "name ") == zox_tst_id_before && !strstr(zox_tst_id_before, "name none") && strstr(zox_tst_id_before, "\nlevel "), "it doesn't start with its name (from its seed) and level [%s]", zox_tst_id_before);
    zox_tst_check(strstr(zox_tst_id_before, "race none") && strstr(zox_tst_id_before, "class none") &&
        strstr(zox_tst_id_before, "job none") && strstr(zox_tst_id_before, "clan none"), "unmade data doesn't read none");
    zox_tst_check(strstr(zox_tst_id_after, "race Elf") != NULL, "a linked race reads [%s], not its name", zox_tst_id_after);
}
