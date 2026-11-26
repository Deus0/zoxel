// Open the window from a deferred spawn, then drive its row click callbacks.
static char zox_tst_files_folder[256];
static entity zox_tst_files_window;
static int zox_tst_files_step;

static byte zox_tst_write_file(const char* path) {
    FILE* file = fopen(path, "wb");
    if (!file) {
        return 0;
    }
    const size_t written = fwrite("hello", 1, 5, file);
    return fclose(file) == 0 && written == 5;
}

static byte zox_tst_make_files(void) {
    const int length = snprintf(zox_tst_files_folder, sizeof(zox_tst_files_folder),
        "%s/file_window_%u", zox_tst_dir(), get_unique_time_seed());
    if (length < 0 || length >= sizeof(zox_tst_files_folder) ||
        has_path_directory(zox_tst_files_folder) || !zox_mkdir(zox_tst_files_folder))
    {
        zox_tst_files_folder[0] = '\0';
        return 0;
    }
    char path[sizeof(zox_tst_files_folder) + 16];
    snprintf(path, sizeof(path), "%s/sub", zox_tst_files_folder);
    if (!zox_mkdir(path)) {
        return 0;
    }
    snprintf(path, sizeof(path), "%s/sub/a.txt", zox_tst_files_folder);
    if (!zox_tst_write_file(path)) {
        return 0;
    }
    snprintf(path, sizeof(path), "%s/b.txt", zox_tst_files_folder);
    return zox_tst_write_file(path);
}

static entity zox_tst_file_rows(ecs* world, entity list, const char* const* labels, int count, int click) {
    entity selected = 0;
    int rows = 0;
    iter children = zox_children(world, list);
    while (zox_children_next(children)) {
        for (int i = 0; i < children.count; i++) {
            const entity row = children.entities[i];
            const uint index = zox_getv(row, ChildIndex);
            const entity text = zox_get_child_by_id(world, row, zox_id(TextData));
            zox_tst_check(index < count && text && is_zext((TextData*) zox_get(text, TextData), labels[index]),
                "wrong file row at index [%u], expected [%s]", index, index < count ? labels[index] : "");
            if (index == click) {
                selected = row;
            }
            rows++;
        }
    }
    zox_tst_check(rows == count, "expected [%i] file rows, got [%i]", count, rows);
    return selected;
}

static byte zox_tst_file_window_ready(ecs* world) {
    if (!zox_tst_files_window) {
        const entity canvas = get_editor_canvas(world);
        if (!canvas) {
            zox_tst_waiting = "the editor canvas";
            return 0;
        }
        const byte made = zox_tst_make_files();
        zox_tst_check(made, "couldn't create the file window fixture");
        if (!made) {
            return 1;
        }
        zox_defer_begin();
        zox_tst_files_window = spawn_window_files(world, canvas, 0, zox_tst_files_folder);
        zox_defer_end();
    }

    const entity list = zox_tst_editor_list(world, zox_tst_files_window);
    zox_tst_check(list, "file window has no list");
    if (!list) {
        return 1;
    }
    if (zox_has(list, Dirty)) {
        return 0;
    }
    static const char* const labels[][3] = {
        { "..", "sub/", "b.txt" },
        { "..", "a.txt" },
        { "..", "a.txt", "a.txt | 5 bytes | txt" },
        { "..", "sub/", "b.txt" },
    };
    const int step = zox_tst_files_step++;
    const entity row = zox_tst_file_rows(world, list, labels[step], step == 1 ? 2 : 3, step < 2 ? 1 : 0);
    if (step == 1 || step == 3) {
        zox_tst_check(zox_getv(list, ListStart) == 0, "navigation kept the old scroll position");
    }
    if (step == 3) {
        return 1;
    }
    const ClickEvent* click = row ? zox_get(row, ClickEvent) : NULL;
    zox_tst_check(click && click->value, "file row has no click callback at step [%i]", step);
    if (!click || !click->value) {
        return 1;
    }
    if (step == 0) {
        zox_setv(list, ListStart, 1);
    }
    click->value(world, (ClickEventData) { .clicked = row });
    return 0;
}

static void zox_tst_file_window_check(ecs* world) {
    (void) world;
    if (zox_tst_files_folder[0]) {
        zox_tst_check(delete_dir(zox_tst_files_folder), "couldn't remove the file window fixture");
    }
    zox_tst_check(zox_log_errors_count == 0, "[%u] errors logged", zox_log_errors_count);
}
