// tsts: hierarchy_search and files_search, an editor window's search bar (src/ui/windows/sys/search.c) filters its
//  rows: by name, by tag (the hierarchy: an id's name, the files: the extension), all rows back when it's cleared

// the open search bar's field and type dropdown
static entity zox_tst_search_field(ecs* world, entity window) {
    const entity bar = window_search_bar_shown(world, window);
    return bar ? zox_get_child_by_id(world, bar, zox_id(TextInput)) : 0;
}

static void zox_tst_search_set(ecs* world, entity window, const char* text, byte type) {
    const entity bar = window_search_bar_shown(world, window);
    const entity dropdown = bar ? zox_get_child_by_id(world, bar, zox_id(Dropdown)) : 0;
    const entity item = dropdown ? item_list_item_at(world, dropdown, type) : 0;
    if (item && zox_get_link(world, dropdown, DropdownSelectedLink) != item) {
        dropdown_set_selected(world, dropdown, item);
    }
    input_field_set_value(world, zox_tst_search_field(world, window), text);
}

// the hierarchy's rows: their count, and the first row's target
static int zox_tst_search_rows(ecs* world, entity window, entity* first) {
    const entity list = zox_tst_editor_list(world, window);
    entity slots[UINT8_MAX + 1];
    *first = get_list_children_ordered(world, list, slots, UINT8_MAX + 1)
        ? zox_get_link(world, slots[0], RowTargetLink) : 0;
    return zox_tst_hierarchy_count(world, window);
}

byte zox_tst_hierarchy_search_ready(ecs* world) {
    static int step;
    static entity window, parent, tagged, root;
    static byte root_opened;
    const entity canvas = get_editor_canvas(world);
    if (!zox_valid(canvas)) {
        zox_tst_waiting = "the editor canvas";
        return 0;
    }
    entity first = 0;
    const int rows = window && !zox_has(window, Dirty) ? zox_tst_search_rows(world, window, &first) : 0;
    zox_tst_progress(step * 100 + rows);
    switch (step) {
        case 0: {
            // root > parent > two children, one of them tagged
            root = zox_new();
            zox_set_name(root, "search_root");
            parent = zox_new();
            zox_set_name(parent, "search_parent");
            zox_set_parent(world, parent, root);
            for (int i = 0; i < 2; i++) {
                const entity child = zox_new();
                zox_set_parent(world, child, parent);
                tagged = child;
            }
            zox_add(tagged, DebugEntity);   // a tag to search for (EditorElement rows are skipped)
            zox_defer_begin();
            window = spawn_window_hierarchy(world, canvas, 0, root);
            zox_defer_end();
            break;
        }
        case 1:
            zox_tst_waiting = "the root and its folded parent";
            if (rows == 1 && !root_opened) {
                root_opened = 1;    // the root starts folded: unfold it
                unfold_toggle(world, zox_tst_editor_list(world, window), root);
                return 0;
            }
            if (rows != 2) {
                return 0;
            }
            window_search_set_open(world, window, 1);
            break;
        case 2:
            zox_tst_waiting = "the search bar";
            if (!zox_tst_search_field(world, window)) {
                return 0;
            }
            zox_tst_search_set(world, window, "Parent", window_search_name);
            break;
        case 3:
            zox_tst_waiting = "only the parent, by name";
            if (rows != 1 || first != parent) {
                return 0;
            }
            zox_tst_search_set(world, window, "debugent", window_search_tag);
            break;
        case 4:
            zox_tst_waiting = "only the tagged child, by tag";
            if (rows != 1 || first != tagged) {
                return 0;
            }
            zox_tst_search_set(world, window, "", window_search_tag);
            break;
        case 5:
            zox_tst_waiting = "the folded tree back, search cleared";
            return rows == 2;
    }
    step++;
    return 0;
}

void zox_tst_hierarchy_search_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_log_errors_count == 0, "[%u] errors logged", zox_log_errors_count);
}

// do the list's rows show these labels, in order (no checks: waiting on a refresh)
static byte zox_tst_files_shown(ecs* world, entity list, const char* const* labels, int count) {
    int rows = 0;
    iter children = zox_children(world, list);
    while (zox_children_next(children)) {
        for (int i = 0; i < children.count; i++) {
            const uint index = zox_getv(children.entities[i], ChildIndex);
            const entity text = zox_get_child_by_id(world, children.entities[i], zox_id(TextData));
            if (index >= count || !text || !is_zext((TextData*) zox_get(text, TextData), labels[index])) {
                return 0;
            }
            rows++;
        }
    }
    return rows == count;
}

byte zox_tst_files_search_ready(ecs* world) {
    static int step;
    static entity window;
    const char* const all[] = { "..", "sub/", "b.txt" };
    const char* const by_name[] = { "..", "sub/" };
    const char* const by_tag[] = { "..", "b.txt" };
    const entity canvas = get_editor_canvas(world);
    if (!zox_valid(canvas)) {
        zox_tst_waiting = "the editor canvas";
        return 0;
    }
    const entity list = window ? zox_tst_editor_list(world, window) : 0;
    const int rows = list && !zox_has(list, Dirty) ? (int) zox_get_children_count(world, list) : 0;
    zox_tst_progress(step * 100 + rows);
    switch (step) {
        case 0:
            if (!zox_tst_make_files()) {
                zox_tst_check(0, "couldn't make the test folder");
                return 1;
            }
            zox_defer_begin();
            window = spawn_window_files(world, canvas, 0, zox_tst_files_folder);
            zox_defer_end();
            break;
        case 1:
            zox_tst_waiting = "the folder's rows";
            if (rows != 3) {
                return 0;
            }
            zox_tst_file_rows(world, list, all, 3, -1);
            window_search_set_open(world, window, 1);
            break;
        case 2:
            zox_tst_waiting = "the search bar";
            if (!zox_tst_search_field(world, window)) {
                return 0;
            }
            zox_tst_search_set(world, window, "S", window_search_name);
            break;
        case 3:
            zox_tst_waiting = "the rows matching s, by name";
            if (!zox_tst_files_shown(world, list, by_name, 2)) {
                return 0;
            }
            zox_tst_search_set(world, window, "txt", window_search_tag);
            break;
        case 4:
            zox_tst_waiting = "the txt files, by tag";
            return zox_tst_files_shown(world, list, by_tag, 2);
    }
    step++;
    return 0;
}

void zox_tst_files_search_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_log_errors_count == 0, "[%u] errors logged", zox_log_errors_count);
    if (zox_tst_files_folder[0]) {
        delete_dir(zox_tst_files_folder);
    }
}

// inspector_search: the inspector of an entity with a name and a tag; its tag search leaves the tag's row only
byte zox_tst_inspector_search_ready(ecs* world) {
    static int step, all;
    static entity window;
    const entity canvas = get_editor_canvas(world);
    if (!zox_valid(canvas)) {
        zox_tst_waiting = "the editor canvas";
        return 0;
    }
    const entity list = window ? zox_tst_editor_list(world, window) : 0;
    const int rows = list && !zox_has(window, InspectorDirty) ? (int) zox_get_children_count(world, list) : 0;
    zox_tst_progress(step * 100 + rows);
    switch (step) {
        case 0: {
            const entity target = zox_new();
            zox_set_name(target, "inspected");
            zox_add(target, EditorElement);
            zox_defer_begin();
            window = spawn_inspector(world, canvas, 0, target);
            zox_defer_end();
            break;
        }
        case 1:
            zox_tst_waiting = "the inspector's rows";
            if (rows < 4) {
                return 0;
            }
            all = rows;
            window_search_set_open(world, window, 1);
            break;
        case 2:
            zox_tst_waiting = "the search bar";
            if (!zox_tst_search_field(world, window)) {
                return 0;
            }
            zox_tst_search_set(world, window, "editorelem", window_search_tag);
            break;
        case 3:
            zox_tst_waiting = "only the tag's row";
            if (rows != 1) {
                return 0;
            }
            zox_tst_search_set(world, window, "", window_search_tag);
            break;
        case 4:
            zox_tst_waiting = "every row back";
            return rows == all;
    }
    step++;
    return 0;
}
