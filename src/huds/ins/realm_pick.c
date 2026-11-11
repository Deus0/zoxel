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
    SaveRealm realm_save;
    load_file_struct(
        path->value,
        "seed.dat",
        &realm_save,
        sizeof(SaveRealm));
    // TODO: Spawn a realm here and window for it
    entity realm = spawn_realm(
        world,
        game,
        realm_save.seed);
    zox_set_ptr(realm, FolderPath, path_copy);
    delay_event(
        world,
        &delay_spawn_menu_realm,
        player,
        0.01);
}

// newest save first, so a capped list keeps the latest saves
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
            sorted[i] = (SaveDirSort) { save_dirs[i], get_save_modified_time(save_dirs[i]) };
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
            // NOTE: the list takes save_path (frees it), text is freed below
            elements[elements_count++] = (SpawnListElement) {
                .text = folder,
                .save_path = sorted[i].path,
                .on_click = { &button_event_load_confirm },
            };
            zox_logv("Save Dir: %s - folder %s", sorted[i].path, folder);
        }
        free(save_dirs);
    }
    entity e = spawn_window_list(
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
    }
    return e;
}
