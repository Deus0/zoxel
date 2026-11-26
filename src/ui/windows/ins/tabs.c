// A window of tabs: a strip of tab buttons over its body, a list for each tab (a scrollview each, spawned with the
//  window), one shown at a time; picking a tab hides the others, nothing respawns (tst options_tabs)
//  - its tab buttons (TabButton) target their tab's scrollview (Target), the strip is ActiveSingle: the picked one's
//    active outline, the last one's off (interaction/sys/link.c)
//  - hidden: RenderDisabled down its scrollview, unseen and not raycast or navigated to (todo: a tag)
//  - the window links the shown tab's scrollview (ScrollviewLink), as a window list links its one
//  - the strip's ClickEvent, if set, is called with the tab picked (its ChildIndex): its owner remembers it

// a scrollview shown or hidden: all of it, its list's elements as its scroll shows them (ListStart, ListVisible)
static inline void window_tab_view_show(ecs* world, entity scrollview, byte shown) {
    set_children_render_disabled(world, scrollview, !shown);
    zox_set(scrollview, RenderDisabled, { !shown });    // its list hides with it (list_render_dirty_system)
    const entity list = zox_get_link(world, scrollview, ListUILink);
    // its scrollbar, shown with it if its list scrolls
    const entity scrollbar = zox_get_child_by_id(world, scrollview, zox_id(Scrollbar));
    const entity handle = get_scrollview_handle(world, scrollview);
    const byte scrolls = zox_valid(list) && zox_has(list, ListVisible) &&
        (int) zox_get_children_count(world, list) > zox_getv(list, ListVisible);
    if (zox_valid(scrollbar)) {
        zox_set(scrollbar, RenderDisabled, { !shown || !scrolls });
    }
    if (zox_valid(handle)) {
        zox_set(handle, RenderDisabled, { !shown || !scrolls });
    }
    if (!shown || !zox_valid(list) || !zox_has(list, ListStart) || !zox_has(list, ListVisible)) {
        return;
    }
    zox_add(list, ListPositionDirty);
    const int start = zox_getv(list, ListStart), visible = zox_getv(list, ListVisible);
    entity children[layouts2_children_capacity];
    const uint count = get_list_children_ordered(world, list, children, layouts2_children_capacity);
    for (uint k = 0; k < count; k++) {
        set_children_render_disabled(world, children[k], !((int) k >= start && (int) k < start + visible));
    }
}

// shows the tab's scrollview, hides the strip's others, the window linking it
static inline void window_tabs_show(ecs* world, entity strip, entity tab) {
    const entity window = zox_get_parent(world, zox_get_parent(world, strip));
    iter it = zox_children(world, strip);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            const entity button = it.entities[i];
            if (!zox_has(button, TabButton)) {
                continue;
            }
            const entity view = zox_getv(button, Target);
            if (zox_valid(view)) {
                window_tab_view_show(world, view, button == tab);
            }
            if (button == tab && zox_valid(window) && zox_valid(view)) {
                zox_set_link(world, window, ScrollviewLink, view);
            }
        }
    }
}

// a tab button clicked: its tab shown, it the strip's active one, the strip's owner told
void on_click_window_tab(ecs* world, const ClickEventData event) {
    const entity tab = event.clicked;
    const entity strip = zox_get_parent(world, tab);
    if (!zox_valid(strip)) {
        return;
    }
    window_tabs_show(world, strip, tab);
    zox_setv(tab, ActiveState, 1);
    zox_add(tab, ActiveStateDirty);
    if (zox_has(strip, ClickEvent) && zox_getv(strip, ClickEvent)) {
        void (*on_tab)(ecs*, ClickEventData) = zox_getv(strip, ClickEvent);
        on_tab(world, event);
    }
}

// a window's tabs a frame on: spawned in a click (a system's, deferred), its scrollviews' children weren't there to
//  hide yet
void window_tabs_show_delayed(ecs* world, entity strip) {
    if (!zox_valid(strip)) {
        return;
    }
    // its active tab (ActiveState: the strip's ActiveLink may not be set yet)
    iter it = zox_children(world, strip);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            const entity button = it.entities[i];
            if (zox_has(button, TabButton) && zox_getv(button, ActiveState)) {
                window_tabs_show(world, strip, button);
                return;
            }
        }
    }
}

// Returns the window, its tab strip; each tab's spawned elements in tab_spawned[tab] (as spawn_window_list's)
//  - tabs: tab_names, their elements and counts; active: the tab shown first; on_tab: the strip's ClickEvent (or 0)
//  - sized to its biggest tab: every tab's list at the same place, under the strip
entity2 spawn_window_tabs(
    ecs* world,
    entity prefab,
    entity canvas,
    const char* header,
    byte header_font_size,
    byte list_font_size,
    ClickEvent close_event,
    byte alignment,
    float2 position_anchor,
    byte2 padding,
    byte tabs_count,
    const char** tab_names,
    SpawnListElement** tab_elements,
    const byte* tab_counts,
    byte visible_count,
    byte active,
    ClickEvent on_tab,
    entity** tab_spawned)
{
    if (!zox_valid(canvas) || !tabs_count) {
        zox_loge("Invalid canvas or no tabs in [spawn_window_tabs]");
        return (entity2) { 0, 0 };
    }
    const byte2 header_padding = (byte2) { 10 * ui_scale, 4 * ui_scale };
    const byte scrollbar_width = 10 * ui_scale;
    // each tab's list, and the biggest
    SpawnList lists[tabs_count];
    int2 list_sizes[tabs_count], scrollview_sizes[tabs_count];
    byte scrollbars[tabs_count];
    int2 size = int2_zero;
    // the most a list may be high: the canvas less its header, the strip (at its biggest) and a margin each side; fewer
    //  rows shown if over (its rows scroll): at a big ui scale the window left the canvas (tst ui_layout)
    const int max_list_height = zox_getv(canvas, LayoutSize).y - 4 * (int) (4 * ui_scale) -
        calculate_header_size(strlen(header), header_font_size, header_padding).y -
        (list_font_size + 2 * (int) (3 * ui_scale) + 2 * (int) (4 * ui_scale));
    for (byte t = 0; t < tabs_count; t++) {
        byte visible = int_min(visible_count, int_max(1, tab_counts[t]));
        int2 tab_size;
        for (;;) {
            lists[t] = window_list_data(tab_elements[t], tab_counts[t], visible, list_font_size, padding);
            window_list_fit(world, canvas, header, &header_font_size, header_padding, &lists[t], 0, scrollbar_width,
                &list_sizes[t], &scrollview_sizes[t], &scrollbars[t], &tab_size);
            if (visible <= 1 || list_sizes[t].y <= max_list_height) {
                break;
            }
            visible--;
        }
        size.x = int_max(size.x, tab_size.x);
        size.y = int_max(size.y, tab_size.y);
    }
    // the strip: the tabs fit its width, their font smaller if not
    const byte2 button_padding = (byte2) { 6 * ui_scale, 3 * ui_scale };
    const int margin = 4 * ui_scale;
    int characters = 0;
    for (byte t = 0; t < tabs_count; t++) {
        characters += strlen(tab_names[t]);
    }
    byte font_size = list_font_size;
    const int fixed = tabs_count * button_padding.x * 2 + (tabs_count + 1) * margin;
    if (characters && characters * font_size + fixed > size.x) {
        font_size = (byte) int_max(4, (size.x - fixed) / characters);
    }
    const int strip_height = font_size + button_padding.y * 2 + margin * 2;
    size.y += strip_height;
    const entity3 e3 = spawn_window(world, prefab, prefab_body, header, canvas, int2_zero, size, position_anchor,
        header_font_size, header_padding, close_event.value);
    const entity window = e3.x;
    const entity body = e3.z;
    const int2 strip_size = { size.x, strip_height };
    const entity strip = spawn_uic(world, prefab_element_frame, body, (float2) { 0.5f, 1 },
        (int2) { 0, -strip_height / 2 }, strip_size, strip_size, window_fill, window_outline);
    zox_set_unique_name(strip, "tabs");
    zox_add(strip, ActiveSingle);
    zox_set(strip, ClickEvent, { on_tab.value });
    // the buttons evenly across it, by their widths
    int width = 0;
    for (byte t = 0; t < tabs_count; t++) {
        width += strlen(tab_names[t]) * font_size + button_padding.x * 2;
    }
    const int gap = (size.x - width) / (tabs_count + 1);
    int x = gap;
    for (byte t = 0; t < tabs_count; t++) {
        // its list, top aligned under the strip
        const int top = size.y / 2 - strip_height;
        const int2 at = { 0, top - scrollview_sizes[t].y / 2 };
        const entity2 view = spawn_window_list_view(world, canvas, body, at, lists[t], list_sizes[t],
            scrollview_sizes[t], scrollbars[t], scrollbar_width, alignment, tab_spawned ? tab_spawned[t] : NULL);
        const int button_width = strlen(tab_names[t]) * font_size + button_padding.x * 2;
        const entity2 button = spawn_button(world, prefab_button, strip, tab_names[t], (int2) { x + button_width / 2, 0 },
            int2_zero, (float2) { 0, 0.5f }, zox_alignment_centre, font_size, button_padding, button_fill,
            button_outline, button_font_fill, button_font_outline);
        zox_add(button.x, TabButton);
        zox_setv(button.x, ClickEvent, &on_click_window_tab);
        zox_set(button.x, Target, { view.x });
        zox_setv(button.x, ChildIndex, t);
        zox_set(button.x, ActiveColor, { button_outline_active });
        zox_set(button.x, ActiveState, { t == active });
        if (t == active) {
            zox_add(button.x, ActiveStateDirty);
            zox_set_link(world, window, ScrollviewLink, view.x);
        } else {
            zox_setv(view.x, RenderDisabled, 1);
        }
        x += button_width + gap;
    }
    // the others hidden through, once they're there
    delay_event(world, &window_tabs_show_delayed, strip, 0);
    return (entity2) { window, strip };
}
