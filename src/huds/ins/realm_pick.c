void button_event_load_cancel(
    ecs *world,
    ClickEventData event)
{
    entity player = event.clicker;
    entity canvas = zox_get_link(world, player, CanvasLink);
    entity menu = zox_get_child_by_id(world, canvas, zox_id(MenuLoad));
    if (!menu) {
        zox_logw("MenuLoad menu not found");
    } else {
        zox_delete(menu);
    }
    spawn_main_menu(
        world,
        player,
        game_name);
}

void delay_spawn_menu_realm(ecs* world, entity player) {
    spawn_menu_realm(world, player);
}

// 3D huds (XR) skip the realm menu (its seed, forget, enter): straight in
void delay_enter_loaded_realm(ecs* world, entity player) {
    const entity game = zox_get_link(world, player, GameLink);
    if (zox_valid(game)) {
        zox_set(game, GameStateTarget, { zox_game_load_start });
    }
}

void button_event_load_confirm(
    ecs *world,
    ClickEventData event)
{
    entity player = event.clicker;
    entity clicked = event.clicked;
    entity game = zox_get_link(world, player, GameLink);
    // copy it, the clicked button is deleted with the menu below (use after free, garbage realm path)
    FolderPath path_copy = *zox_get(clicked, FolderPath);
    const FolderPath* path = &path_copy;
    entity canvas = zox_get_link(world, player, CanvasLink);
    entity menu = zox_get_child_by_id(world, canvas, zox_id(MenuLoad));
    if (!menu) {
        zox_loge("UI [MenuLoad] Not Found");
        return;
    }
    zox_delete(menu);
    zox_logv("Save Realm Path [%s]", path->value);
    RealmInfo info;
    load_realm_info(path->value, &info);
    // played from now, and last played now (realm.txt)
    realm_played_since = 0;
    realm_info_played(path->value, zox_current_time);
    // TODO: Spawn a realm here and window for it
    entity realm = spawn_realm(
        world,
        game,
        info.seed);
    zox_set_ptr(realm, FolderPath, path_copy);
    delay_event(
        world,
        zox_huds3D ? &delay_enter_loaded_realm : &delay_spawn_menu_realm,
        player,
        0.01);
}

// last played first (realm.txt, else its folder's time), so a capped list keeps the latest
static inline time_t get_save_modified_time(const char* path) {
    struct stat st;
    if (stat(path, &st) != 0) {
        return 0;
    }
    return st.st_mtime;
}

typedef struct {
    char* path;
    time_t modified;
    RealmInfo info;
} SaveDirSort;

static int compare_save_dirs_newest(const void* a, const void* b) {
    const time_t ta = ((const SaveDirSort*) a)->modified;
    const time_t tb = ((const SaveDirSort*) b)->modified;
    if (ta != tb) {
        return ta < tb ? 1 : -1;
    }
    // same time, keep a stable name order
    return strcmp(((const SaveDirSort*) a)->path, ((const SaveDirSort*) b)->path);
}

// 3D huds: the newest saves (no scrolling a 3D list yet) and back, each save's button its FolderPath
#define menu_load3_saves 6

entity spawn_menu_load3(ecs* world, entity player, const char* header, byte header_font_size, byte font_size,
    SpawnListElement* elements, int count)
{
    SpawnListElement list[menu_load3_saves + 1];
    byte listed = 0;
    for (int i = 0; i < count && listed < menu_load3_saves; i++) {
        list[listed++] = elements[i];
    }
    list[listed++] = (SpawnListElement) {
        .text = "Back",
        .on_click = { &button_event_load_cancel },
    };
    entity spawned[menu_load3_saves + 1];
    const entity e = spawn_huds3_list(world, player, header, header_font_size, font_size, list, listed, spawned);
    for (int i = 0; i < count; i++) {
        if (i < listed - 1) {
            FolderPath path = { };
            snprintf(path.value, sizeof(path.value), "%s", elements[i].save_path);
            zox_set_ptr(spawned[i], FolderPath, path);
        }
        free(elements[i].save_path);
        elements[i].save_path = NULL;
    }
    return e;
}

entity spawn_menu_load(ecs *world, entity player) {
    const char* header_label = "Which";
    byte visible_count = 4;
    float2 window_anchor = zox_huds_window_anchor;
    byte header_font_size = zox_huds_header_font_size * ui_scale;
    byte list_font_size = zox_huds_element_font_size * ui_scale;
    byte2 padding = byte2_single(zox_huds_list_padding * ui_scale);
    SpawnListElement elements[huds_max_list_elements];
    int elements_count = 0;
    char **save_dirs;
    byte saves_count;
    if (!get_save_games(game_name, &save_dirs, &saves_count)) {
        SaveDirSort sorted[256];
        for (int i = 0; i < saves_count; i++) {
            sorted[i] = (SaveDirSort) { .path = save_dirs[i] };
            load_realm_info(save_dirs[i], &sorted[i].info);
            sorted[i].modified = sorted[i].info.last ? (time_t) sorted[i].info.last : get_save_modified_time(save_dirs[i]);
        }
        qsort(sorted, saves_count, sizeof(SaveDirSort), compare_save_dirs_newest);
        if (saves_count > huds_max_list_elements) {
            zox_logw("[%i] saves, the load menu only lists the newest [%i]",
                saves_count,
                huds_max_list_elements);
        }
        for (int i = 0; i < saves_count; i++) {
            // NOTE: elements is fixed size, overflowing it used to corrupt the stack (crash on continue)
            char* folder = elements_count < huds_max_list_elements ?
                get_folder_name(sorted[i].path) :
                NULL;
            if (!folder) {
                free(sorted[i].path);
                continue;
            }
            // its name (older saves: its folder), its seed and time played hovering it
            const RealmInfo* info = &sorted[i].info;
            if (info->name[0]) {
                free(folder);
                folder = strdup(info->name);
            }
            char played[32];
            realm_played_text(info->played, played, sizeof(played));
            char* tooltip = malloc(64);
            snprintf(tooltip, 64, "seed %u\nplayed %s", info->seed, played);
            // NOTE: the list takes save_path (frees it), text and tooltip are freed below
            elements[elements_count++] = (SpawnListElement) {
                .text = folder,
                .tooltip = tooltip,
                .save_path = sorted[i].path,
                .on_click = { &button_event_load_confirm },
            };
            zox_logv("Save Dir: %s - folder %s", sorted[i].path, folder);
        }
        free(save_dirs);
    }
    entity e;
    if (zox_huds3D) {
        e = spawn_menu_load3(world, player, header_label, header_font_size, list_font_size, elements, elements_count);
    } else e = spawn_window_list(
        world,
        prefab_window,
        player,
        header_label,
        header_font_size,
        list_font_size,
        (ClickEvent) { &button_event_load_cancel },
        1,
        0,
        0,
        zox_alignment_centre,
        window_anchor,
        padding,
        NULL,
        elements,
        elements_count,
        visible_count).x;
    zox_name("menu_load");
    zox_add(e, MenuLoad);
    zox_add(e, NavigationWindow);
    // button labels copy their text
    for (int i = 0; i < elements_count; i++) {
        free((char*) elements[i].text);
        free((char*) elements[i].tooltip);
    }
    return e;
}
