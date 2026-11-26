static void click_file_window_row(ecs* world, ClickEventData event, byte directory) {
    const entity row = event.clicked;
    const entity window = zox_get_parent_by_id(world, row, zox_id(FileWindowUI));
    if (!window) {
        return;
    }
    const entity list = zox_get_parent(world, row);
    const FolderPath* path = zox_get(row, FolderPath);
    if (directory) {
        set_FolderPath(world, list, "");
        set_FolderPath(world, window, path->value);
        zox_setv(list, ListStart, 0);
    } else {
        set_FolderPath(world, list, path->value);
    }
    zox_add(list, Dirty);
}

static void click_file_window_folder(ecs* world, ClickEventData event) {
    click_file_window_row(world, event, 1);
}

static void click_file_window_file(ecs* world, ClickEventData event) {
    click_file_window_row(world, event, 0);
}

static void spawn_file_window_row(
    ecs* world, entity list, byte font_size, uint index,
    const char* label, const char* path, ClickEvent click)
{
    const entity row = spawn_editor_list_row(
        world, list, prefab_button, label, font_size, editor_button_outline, index, 0);
    if (click.value) {
        set_FolderPath(world, row, path);
        zox_setv(row, ClickEvent, click.value);
    }
}

// a search: the entry's name (name), or a file's extension (tag)
static byte file_window_search_match(DirectoryEntry entry, const char* search, byte type) {
    const char* name = file_window_basename(entry.path);
    if (type != window_search_tag) {
        return window_search_match(name, search);
    }
    const char* dot = strrchr(name, '.');
    return !entry.directory && dot && dot != name && window_search_match(dot + 1, search);
}

static void spawn_file_window_entries(ecs* world, entity list, byte font_size, FolderPath folder) {
    const ClickEvent open_folder = { click_file_window_folder };
    if (!folder.value[0]) {
        const FolderPath resources = file_window_path(resources_path);
        const FolderPath saves = file_window_saves();
        spawn_file_window_row(world, list, font_size, 0, "Resources/", resources.value, open_folder);
        spawn_file_window_row(world, list, font_size, 1, "Saves/", saves.value, open_folder);
        return;
    }

    const FolderPath parent = file_window_parent(folder);
    spawn_file_window_row(world, list, font_size, 0, "..", parent.value, open_folder);
    size_t count;
    DirectoryEntry* entries = get_directory_entries(folder.value, &count);
    // The shared list lays out at most this many rows. Leave room for file info.
    const uint entry_limit = layouts2_children_capacity - (count > layouts2_children_capacity - 2 ? 2 : 1);
    uint index = 1;
    byte omitted = 0;
    char search[input_field_max_length + 2];
    byte search_type;
    const entity window = zox_get_parent_by_id(world, list, zox_id(FileWindowUI));
    const byte searching = window && window_search_get(world, window, search, sizeof(search), &search_type) &&
        search[0];
    for (size_t j = 0; j < count; j++) {
        const DirectoryEntry entry = entries[j];
        if (searching && !file_window_search_match(entry, search, search_type)) {
            free(entry.path);
            continue;
        }
        if (index < entry_limit && strlen(entry.path) < sizeof(folder.value)) {
            char label[sizeof(folder.value) + 1];
            snprintf(label, sizeof(label), "%s%s", file_window_basename(entry.path), entry.directory ? "/" : "");
            const ClickEvent click = entry.directory ? open_folder : (ClickEvent) { click_file_window_file };
            spawn_file_window_row(world, list, font_size, index++, label, entry.path, click);
        } else {
            omitted = 1;
        }
        free(entry.path);
    }
    free(entries);
    if (omitted) {
        spawn_file_window_row(world, list, font_size, index++, "More entries...", NULL, (ClickEvent) { 0 });
    }

    const FolderPath* selected = zox_get(list, FolderPath);
    struct stat info;
    if (selected->value[0] && !stat(selected->value, &info) && S_ISREG(info.st_mode)) {
        const char* name = file_window_basename(selected->value);
        const char* dot = strrchr(name, '.');
        const char* extension = dot && dot != name && dot[1] ? dot + 1 : "(none)";
        char label[2 * sizeof(folder.value) + 32];
        snprintf(label, sizeof(label), "%s | %lld bytes | %s", name, (long long) info.st_size, extension);
        spawn_file_window_row(world, list, font_size, index, label, NULL, (ClickEvent) { 0 });
        const int visible = zox_getv(list, ListVisible);
        if (visible && index >= zox_getv(list, ListStart) + visible) {
            zox_setv(list, ListStart, index + 1 - visible);
        }
    }
}

static void file_window_spawn_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        const entity list = it->entities[i];
        const entity window = zox_get_parent_by_id(world, list, zox_id(FileWindowUI));
        if (window) {
            clear_editor_list(world, list);
            spawn_file_window_entries(world, list, zox_getv(window, ElementFontSize), *zox_get(window, FolderPath));
            refresh_editor_list(world, list);
        }
    }
}
