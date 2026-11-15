// tst: realm_info, realms keep a name, seed, time played and when last played (realm.txt, saves/fun/realm_info.c)
//  - realm.txt saved and loaded back
//  - time played as text (under a minute, 5m, 2h 5m)
//  - the load menu: three realms, its buttons their names, last played first, each one's tooltip its seed and time
//  - run: bash bsh/tst.sh realm_info
int zox_tst_realm_info_stage = 0;
int zox_tst_realm_info_frames = 0;

// the load menu's buttons with a save (FolderPath), in list order
static inline int zox_tst_realm_info_buttons(ecs* world, entity e, entity* buttons, int count) {
    if (zox_has(e, FolderPath) && count < 8) {
        const int index = zox_has(e, ChildIndex) ? zox_getv(e, ChildIndex) : count;
        if (index < 8) {
            buttons[index] = e;
        }
        return count + 1;
    }
    iter it = zox_children(world, e);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            count = zox_tst_realm_info_buttons(world, it.entities[i], buttons, count);
        }
    }
    return count;
}

static inline void zox_tst_realm_info_make(const char* saves, const char* folder_name, RealmInfo info) {
    char folder[512];
    snprintf(folder, sizeof(folder), "%s/%s", saves, folder_name);
    mkdir(folder, 0755);
    save_realm_info(folder, &info);
}

byte zox_tst_realm_info_ready(ecs* world) {
    const entity player = dbg_player;
    const entity canvas = zox_valid(player) ? zox_get_link(world, player, CanvasLink) : 0;
    zox_tst_realm_info_frames++;
    zox_tst_progress(zox_tst_realm_info_stage);
    if (zox_tst_realm_info_stage == 0) {
        zox_tst_waiting = "a player with a canvas";
        if (!zox_valid(canvas) || zox_tst_realm_info_frames < 10) {
            return 0;
        }
        char saves[512];
        get_save_directory(game_name, saves, sizeof(saves));
        char command[600];
        snprintf(command, sizeof(command), "mkdir -p '%s'", saves);
        zox_tst_check(!system(command), "couldn't make [%s]", saves);
        // saved and loaded back
        const long now = (long) time(NULL);
        zox_tst_realm_info_make(saves, "a", (RealmInfo) { .seed = 11, .name = "Old Lands", .played = 7500, .last = now - 300 });
        zox_tst_realm_info_make(saves, "b", (RealmInfo) { .seed = 22, .name = "Newest", .played = 300, .last = now - 10 });
        zox_tst_realm_info_make(saves, "c", (RealmInfo) { .seed = 33, .name = "Middle", .played = 20, .last = now - 100 });
        char folder[512];
        snprintf(folder, sizeof(folder), "%s/a", saves);
        RealmInfo info;
        zox_tst_check(load_realm_info(folder, &info), "realm.txt in [%s] didn't load", folder);
        zox_tst_check(info.seed == 11 && !strcmp(info.name, "Old Lands") && info.played == 7500 && info.last == now - 300,
            "realm.txt loaded back seed [%u] name [%s] played [%u] last [%ld]", info.seed, info.name, info.played, info.last);
        // time played as text
        char text[32];
        realm_played_text(20, text, sizeof(text));
        zox_tst_check(!strcmp(text, "under a minute"), "20 seconds played reads [%s]", text);
        realm_played_text(7500, text, sizeof(text));
        zox_tst_check(!strcmp(text, "2h 5m"), "7500 seconds played reads [%s]", text);
        // the load menu
        const entity main_menu = zox_get_child_by_id(world, canvas, zox_id(MenuMain));
        if (zox_valid(main_menu)) {
            zox_delete(main_menu);
        }
        spawn_menu_load(world, player);
        zox_tst_realm_info_stage = 1;
        zox_tst_realm_info_frames = 0;
        return 0;
    }
    zox_tst_waiting = "the load menu";
    if (zox_tst_realm_info_frames < 10) {
        return 0;
    }
    const entity menu = zox_get_child_by_id(world, canvas, zox_id(MenuLoad));
    zox_tst_check(zox_valid(menu), "no load menu");
    if (!zox_valid(menu)) {
        return 1;
    }
    entity buttons[8] = { 0 };
    const int count = zox_tst_realm_info_buttons(world, menu, buttons, 0);
    const char* expected[] = { "Newest", "Middle", "Old Lands" };
    zox_tst_check(count == 3, "the load menu lists [%i] realms, not 3", count);
    for (int i = 0; i < 3 && i < count; i++) {
        const entity label = zox_get_child_by_id_recursive(world, buttons[i], zox_id(Text));
        char* name = zox_valid(label) ? get_zext_text(zox_get(label, TextData)) : NULL;
        const char* tooltip = zox_has(buttons[i], TooltipText) ? zox_get(buttons[i], TooltipText)->value : "";
        zox_log("[tst] load menu [%i]: [%s] tooltip [%s]", i, name ? name : "-", tooltip);
        zox_tst_check(name && !strcmp(name, expected[i]), "load menu button [%i] is [%s], not [%s]", i, name ? name : "-", expected[i]);
        free(name);
        if (i == 2) {
            zox_tst_check(!strcmp(tooltip, "seed 11\nplayed 2h 5m"), "Old Lands's tooltip is [%s]", tooltip);
        }
    }
    return 1;
}

void zox_tst_realm_info_check(ecs* world) {
    (void) world;
}
