// The editor windows' header icons, drawn here as pixels (white on clear, like the fold arrows)
enum { editor_icon_hierarchy, editor_icon_inspector, editor_icon_files, editor_icons_count };
static entity editor_icons[editor_icons_count];

// is pixel x, y (from the bottom left, of a 15 square) lit in the icon
static byte editor_icon_pixel(int icon, int x, int y) {
    const int top = 14 - y;     // drawn from the top
    if (icon == editor_icon_hierarchy) {
        // a root, a line down from it, two children off the line
        const byte root = x >= 1 && x <= 5 && top >= 1 && top <= 4;
        const byte line = x == 3 && top >= 5 && top <= 11;
        const byte branches = (top == 7 || top == 11) && x >= 3 && x <= 7;
        const byte children = x >= 8 && x <= 13 && ((top >= 6 && top <= 8) || (top >= 10 && top <= 12));
        return root || line || branches || children;
    }
    if (icon == editor_icon_inspector) {
        // a list of properties: a dot and a line each
        const int row = top - 2;
        if (row < 0 || row > 10 || row % 4 == 3) {
            return 0;
        }
        return (x >= 1 && x <= 2) || (x >= 5 && x <= 13 - (row / 4) * 3);
    }
    // a folder: its tab, then its body
    const byte tab = top >= 2 && top <= 3 && x >= 1 && x <= 6;
    const byte body = top >= 4 && top <= 12 && x >= 1 && x <= 13;
    const byte hollow = top >= 6 && top <= 10 && x >= 3 && x <= 11;
    return tab || (body && !hollow);
}

static void spawn_editor_icons(ecs* world) {
    const int side = 15;
    for (int icon = 0; icon < editor_icons_count; icon++) {
        const entity texture = spawn_texture(world, prefab_texture, int2_single(side));
        TextureData pixels = { 0 };
        initialize_TextureData(&pixels, side * side);
        for (int y = 0; y < side; y++) {
            for (int x = 0; x < side; x++) {
                pixels.value[y * side + x] = editor_icon_pixel(icon, x, y) ? editor_color_font : empty_color;
            }
        }
        zox_set_ptr(texture, TextureData, pixels);
        zox_setv(texture, TextureGPULink, 0);
        zox_add(texture, Initialize);
        zox_add(texture, TextureDirty);
        editor_icons[icon] = texture;
    }
}
