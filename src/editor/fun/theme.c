// The editor's look (set/_.c, GitHub's dark theme) on a window or the menubar, once its parts are spawned
//  (EditorThemeDirty, eve/_.c): headers, bodies, scroll views, buttons, menus and their texts. Lists are skipped, their
//  rows are the hierarchy's, inspector's and files' own (an inspector row's colour is its component's)
static void editor_theme_element(ecs* world, entity e, color fill, color outline) {
    zox_setv(e, FillColor, fill);
    zox_setv(e, ElementFillColor, fill);
    zox_setv(e, OutlineColor, outline);
    zox_setv(e, ElementOutlineColor, outline);
    if (zox_has(e, SelectedFillColor)) {
        zox_setv(e, SelectedFillColor, editor_hover_fill);
    }
    if (zox_has(e, GenerateTexture)) {
        zox_setv(e, GenerateTexture, zox_generate_texture_run);
    }
}

static void editor_theme(ecs* world, entity e) {
    if (zox_has(e, ListVisible) || zox_has(e, WindowResizeHandle)) {
        return;
    }
    if (zox_has(e, TextData)) {
        color font = editor_color_font;
        font.a = zox_has(e, FontFillColor) ? zox_getv(e, FontFillColor).a : 255;  // a disabled item's stays dimmed
        set_entity_text_fill(world, e, font);
        return;     // its children are glyphs
    }
    if (zox_has(e, FillColor) && !zox_has(e, Window)) {
        if (zox_has(e, Header) || zox_has(e, Menubar) || zox_has(e, ItemListPopup)) {
            editor_theme_element(world, e, editor_header_fill, editor_header_outline);
        } else if (zox_has(e, WindowBody) || zox_has(e, Scrollview)) {
            editor_theme_element(world, e, editor_window_fill, editor_window_outline);
        } else {
            editor_theme_element(world, e, editor_button_fill, editor_button_outline);
        }
    }
    iter it = zox_children(world, e);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            editor_theme(world, it.entities[i]);
        }
    }
}
