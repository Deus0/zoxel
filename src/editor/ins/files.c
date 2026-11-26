// An empty folder displays the two roots. The list's path holds the selected file.
entity spawn_window_files(ecs* world, entity canvas, entity player, const char* folder) {
    (void) player;
    const int2 canvas_size = zox_getv(canvas, LayoutSize);
    const byte font_size = 12;
    const entity4 ui = spawn_editor_window(world, prefab_files, canvas, "Files", dock_bottom,
        (int2) { canvas_size.x, canvas_size.y / 4 }, font_size);
    zox_set_name_spawned(world, ui.x, "files");
    zox_add(ui.x, FileWindowUI);
    zox_setv(ui.x, ElementFontSize, font_size);
    const FolderPath path = file_window_path(folder);
    zox_set_ptr(ui.x, FolderPath, path);
    set_FolderPath(world, ui.w, "");
    // Queue the list refresh after the window, its links and the list are ready.
    zox_add(ui.w, Dirty);
    return ui.x;
}
