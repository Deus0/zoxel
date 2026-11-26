// a window's search bar (ins/search.c): opening and closing it, and telling the window when the search changes

// its search while its bar is open: 1, its text (lower case) and filter type (window_search_name, _tag), else 0
byte window_search_get(ecs* world, entity window, char* text, int size, byte* type) {
    const entity bar = window_search_bar_shown(world, window);
    if (!bar) {
        return 0;
    }
    char* value = input_field_value(world, zox_get_child_by_id(world, bar, zox_id(TextInput)));
    snprintf(text, size, "%s", value);
    free(value);
    for (char* c = text; *c; c++) {
        *c = tolower(*c);
    }
    const entity dropdown = zox_get_child_by_id(world, bar, zox_id(DropdownIndex));
    *type = dropdown ? zox_getv(dropdown, DropdownIndex) : window_search_name;
    return 1;
}

// does name contain search, ignoring case (search already lower case)
byte window_search_match(const char* name, const char* search) {
    if (!search[0]) {
        return 1;
    }
    char lower[128];
    snprintf(lower, sizeof(lower), "%s", name ? name : "");
    for (char* c = lower; *c; c++) {
        *c = tolower(*c);
    }
    return strstr(lower, search) != NULL;
}

// the window's rows respawn: the core Dirty on its list (the editor's windows observe it)
static void window_search_changed(ecs* world, entity window) {
    const entity scrollview = zox_get_link(world, window, ScrollviewLink);
    const entity list = zox_valid(scrollview) ? zox_get_link(world, scrollview, ListUILink) : 0;
    if (zox_valid(list)) {
        zox_add(list, Dirty);
    }
}

// opens or closes (hides, clearing its text and type) its window's search bar, then the body fits around it
void window_search_set_open(ecs* world, entity window, byte open) {
    entity bar = zox_get_child_by_id(world, window, zox_id(WindowSearchBar));
    if (open && !bar) {
        bar = spawn_window_search_bar(world, window);
    }
    if (!bar) {
        return;
    }
    if (!open) {
        const entity field = zox_get_child_by_id(world, bar, zox_id(TextInput));
        if (field == input_field_focus) {
            input_field_unfocus(world);
        }
        input_field_set_value(world, field, "");
        const entity dropdown = zox_get_child_by_id(world, bar, zox_id(DropdownIndex));
        if (dropdown) {
            dropdown_close(world, dropdown);
            zox_setv(dropdown, DropdownIndex, window_search_name);
        }
    }
    set_children_render_disabled(world, bar, !open);
    zox_add(bar, Dirty);    // window_search_bar_event lays its window out once the bar is set (spawns are deferred)
}

// a search bar opened or closed (its core Dirty): its window body fits around it, its rows respawn
static void window_search_bar_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        const entity window = zox_get_parent(world, it->entities[i]);
        if (zox_valid(window)) {
            window_dock_layout(world, window, zox_getv(window, LayoutPosition), zox_getv(window, LayoutSize));
            window_search_changed(world, window);
        }
    }
}

static void window_search_toggle(ecs* world, ClickEventData event) {
    const entity window = zox_get_parent_by_id(world, event.clicked, zox_id(Window));
    if (window) {
        window_search_set_open(world, window, !window_search_bar_shown(world, window));
    }
}

// typing in a search bar's field or picking its filter type (their core Dirty): its window's rows respawn
static void window_search_input_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        const entity bar = zox_get_parent_by_id(world, it->entities[i], zox_id(WindowSearchBar));
        if (bar && !zox_getv(bar, RenderDisabled)) {
            window_search_changed(world, zox_get_parent(world, bar));
        }
    }
}
