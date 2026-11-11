// tst: load_games, the main menu's continue button lists saves and loads one
//  - setup (before boot): saves root (zox_saves_root) is bin/tst/load_home, never the real saves
//      - makes more saves than the load list holds (it used to overflow and crash on continue)
//      - each save has its own seed, and a newer modified time than the one before
//  - clicks continue on the main menu, checks the load list keeps the newest saves
//  - clicks the newest save, checks its realm is spawned with that save's seed and path
//  - run: bash bsh/tst.sh load_games
#ifndef zox_windows
    #include <utime.h>
#endif
#define zox_tst_load_saves (huds_max_list_elements + 12)
#define zox_tst_load_seed 5000
#define zox_tst_load_settle_frames 5
char zox_tst_load_saves_path[512];
int zox_tst_load_stage = 0;
int zox_tst_load_frames = 0;
entity zox_tst_load_button = 0;
const char* zox_tst_load_game_name = NULL;

static inline int zox_tst_load_save_index(const char* path) {
    const char* name = strrchr(path, '/');
    name = name ? name + 1 : path;
    int index = -1;
    if (sscanf(name, "realm_%d", &index) != 1) {
        return -1;
    }
    return index;
}

void zox_tst_load_games_setup() {
#ifdef zox_windows
    zox_loge("[tst] load_games setup is posix only");
#else
    char cwd[256];
    if (!getcwd(cwd, sizeof(cwd))) {
        zox_loge("[tst] load_games can't get the working directory");
        return;
    }
    char home[384];
    snprintf(home, sizeof(home), "%s/../tst/load_home", cwd);
    char command[1024];
    snprintf(command, sizeof(command), "rm -rf '%s'", home);
    if (system(command) != 0) {
        zox_logw("[tst] couldn't clear [%s]", home);
    }
    // NOTE: saves root is set before --nosaves is processed, which keeps it
    snprintf(zox_saves_root, sizeof(zox_saves_root), "%s", home);
    zox_tst_load_game_name = game_name;
    snprintf(zox_tst_load_saves_path, sizeof(zox_tst_load_saves_path),
        "%s/%s", home, game_name);
    snprintf(command, sizeof(command), "mkdir -p '%s'", zox_tst_load_saves_path);
    if (system(command) != 0) {
        zox_loge("[tst] couldn't make [%s]", zox_tst_load_saves_path);
        return;
    }
    const time_t now = time(NULL);
    for (int i = 0; i < zox_tst_load_saves; i++) {
        char folder[640];
        snprintf(folder, sizeof(folder), "%s/realm_%03i", zox_tst_load_saves_path, i);
        mkdir(folder, 0755);
        char file[700];
        snprintf(file, sizeof(file), "%s/seed.dat", folder);
        FILE* out = fopen(file, "wb");
        if (out) {
            SaveRealm save = { .seed = zox_tst_load_seed + i };
            fwrite(&save, sizeof(SaveRealm), 1, out);
            fclose(out);
        }
        // higher index is newer
        struct utimbuf times = { now - 10000 + i, now - 10000 + i };
        utime(folder, &times);
    }
    zox_log("[tst] made [%i] saves in [%s]", zox_tst_load_saves, zox_tst_load_saves_path);
    // zox_tst_save=<realm folder>: a copy of a real save becomes the newest, i.e. to reproduce a bad load
    //  - only copies from it, the real save is never written to
    const char* real_save = getenv("zox_tst_save");
    if (real_save && real_save[0]) {
        char newest[640];
        snprintf(newest, sizeof(newest), "%s/realm_%03i", zox_tst_load_saves_path, zox_tst_load_saves - 1);
        snprintf(command, sizeof(command), "rm -rf '%s' && cp -r '%s' '%s' && touch '%s'", newest, real_save, newest, newest);
        if (system(command) != 0) {
            zox_loge("[tst] couldn't copy save [%s]", real_save);
        } else {
            zox_log("[tst] newest save is a copy of [%s]", real_save);
        }
    }
#endif
}

// finds a descendant with a ClickEvent of on_click
entity zox_tst_find_click(ecs* world, entity e, void (*on_click)(ecs*, ClickEventData)) {
    if (zox_has(e, ClickEvent) && zox_getv(e, ClickEvent) == on_click) {
        return e;
    }
    iter it2 = zox_children(world, e);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity found = zox_tst_find_click(world, it2.entities[j], on_click);
            if (found) {
                return found;
            }
        }
    }
    return 0;
}

// counts descendants with a FolderPath, and finds the newest save's button
int zox_tst_load_list_buttons(ecs* world, entity e, int* oldest, entity* newest) {
    int count = 0;
    if (zox_has(e, FolderPath)) {
        zox_geter(e, FolderPath, path);
        const int index = zox_tst_load_save_index(path->value);
        if (index < *oldest) {
            *oldest = index;
        }
        if (index == zox_tst_load_saves - 1) {
            *newest = e;
        }
        count++;
    }
    iter it2 = zox_children(world, e);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            count += zox_tst_load_list_buttons(world, it2.entities[j], oldest, newest);
        }
    }
    return count;
}

static inline void zox_tst_load_click(ecs* world, entity player, entity button) {
    void (*on_click)(ecs*, ClickEventData) = zox_getv(button, ClickEvent);
    on_click(world, (ClickEventData) { .clicker = player, .clicked = button });
}

byte zox_tst_load_games_ready(ecs* world) {
    const entity player = dbg_player;
    const entity canvas = zox_valid(player) ? zox_get_link(world, player, CanvasLink) : 0;
    if (zox_tst_load_stage == 0) {
        zox_tst_waiting = "the main menu";
        if (!zox_valid(canvas)) {
            return 0;
        }
        // press any key on the start menu, like a player
        if (zox_valid(zox_get_link(world, player, MenuStart))) {
            menu_start_triggered(world, player, canvas);
            return 0;
        }
        const entity main_menu = zox_get_child_by_id(world, canvas, zox_id(MenuMain));
        if (!zox_valid(main_menu)) {
            return 0;
        }
        zox_tst_check(game_name == zox_tst_load_game_name, "game_name changed after setup, saves are in the wrong folder");
        const entity button = zox_tst_find_click(world, main_menu, &button_event_continue_game);
        zox_tst_check(button, "main menu has no continue button, saves in [%s] weren't found", zox_tst_load_saves_path);
        if (!button) {
            return 1;
        }
        zox_tst_load_click(world, player, button);
        zox_tst_load_stage = 1;
        zox_tst_load_frames = 0;
        return 0;
    }
    zox_tst_load_frames++;
    if (zox_tst_load_frames < zox_tst_load_settle_frames) {
        return 0;
    }
    if (zox_tst_load_stage == 1) {
        zox_tst_waiting = "the load menu";
        const entity menu = zox_get_child_by_id(world, canvas, zox_id(MenuLoad));
        if (!zox_valid(menu)) {
            return 0;
        }
        int oldest = zox_tst_load_saves;
        entity newest = 0;
        const int listed = zox_tst_load_list_buttons(world, menu, &oldest, &newest);
        const int expected = zox_tst_load_saves < huds_max_list_elements ? zox_tst_load_saves : huds_max_list_elements;
        zox_log("[tst] load menu lists [%i] of [%i] saves, oldest listed [realm_%03i]", listed, zox_tst_load_saves, oldest);
        zox_tst_check(listed == expected, "load menu lists [%i] saves, expected [%i]", listed, expected);
        zox_tst_check(oldest == zox_tst_load_saves - expected, "load menu's oldest save is [%i], expected the newest [%i] saves", oldest, expected);
        zox_tst_check(newest, "load menu has no button for the newest save");
        if (!newest) {
            return 1;
        }
        zox_tst_load_button = newest;
        zox_tst_load_click(world, player, newest);
        zox_tst_load_stage = 2;
        zox_tst_load_frames = 0;
        return 0;
    }
    zox_tst_waiting = "the loaded realm";
    const entity game = zox_get_link(world, player, GameLink);
    const entity realm = zox_valid(game) ? zox_get_link(world, game, RealmLink) : 0;
    if (!zox_valid(realm) || !zox_has(realm, FolderPath)) {
        return 0;
    }
    zox_geter(realm, FolderPath, path);
    const int index = zox_tst_load_save_index(path->value);
    const uint seed = (uint) zox_getv(realm, Seed);
    zox_log("[tst] loaded realm [%s] seed [%u] from [%s]", zox_getn(realm), seed, path->value);
    zox_tst_check(index == zox_tst_load_saves - 1, "loaded realm path [%s], expected the newest save", path->value);
    const char* real_save = getenv("zox_tst_save");
    zox_tst_check((real_save && real_save[0]) || seed == zox_tst_load_seed + zox_tst_load_saves - 1, "loaded seed [%u], expected [%i]", seed, zox_tst_load_seed + zox_tst_load_saves - 1);
    zox_tst_check(!zox_valid(zox_get_child_by_id(world, canvas, zox_id(MenuLoad))), "load menu wasn't closed");
    return 1;
}

void zox_tst_load_games_check(ecs* world) {
    (void) world;
}

void zox_tsts_huds() {
    zox_tst_add("load_games", zox_tst_load_games_ready, zox_tst_load_games_check, 60);
    zox_tst_set_setup("load_games", zox_tst_load_games_setup, 1);
}
