// Scrollviews: a Scrollview has a Scrollbar child (with a ScrollbarHandle) and a list (ListUILink)
//  - ListStart is the list's first shown element, ListVisible how many show at once
//  - dragging the handle (scrollbar_system) and the mouse wheel (scrollview_wheel_system) set the start

// the furthest the list can scroll, 0 if every element fits
int get_scrollview_max_start(
    ecs* world,
    entity list)
{
    const int count = (int) zox_get_children_count(world, list);
    const int visible = zox_getv(list, ListVisible);
    return count > visible ? count - visible : 0;
}

entity get_scrollview_handle(
    ecs* world,
    entity scrollview)
{
    const entity scrollbar = zox_get_child_by_id(world, scrollview, zox_id(Scrollbar));
    if (!zox_valid(scrollbar)) {
        return 0;
    }
    return zox_get_child_by_id(world, scrollbar, zox_id(ScrollbarHandle));
}

// moves the handle along its scrollbar to show start, the inverse of scrollbar_system
void set_scrollbar_handle_to_start(
    ecs* world,
    entity scrollview,
    int start,
    int max_start)
{
    const entity handle = get_scrollview_handle(world, scrollview);
    if (!zox_valid(handle) || max_start <= 0) {
        return;
    }
    const entity scrollbar = zox_get_parent(world, handle);
    const int travel = zox_getv(scrollbar, LayoutSize).y - zox_getv(handle, LayoutSize).y;
    if (travel <= 0) {
        return;
    }
    // the top of the scrollbar is start 0
    const float percentage = start / (float) max_start;
    int2 position = zox_getv(handle, LayoutPosition);
    position.y = travel / 2 - (int) round(percentage * travel);
    zox_setv(handle, LayoutPosition, position);
}

// scrolls a list to start (clamped), returns 1 if it moved: list_system places its elements from start,
//  list_render_dirty_system shows ListVisible of them
byte set_list_start(
    ecs* world,
    entity list,
    int start)
{
    if (!zox_valid(list) ||
        !zox_has(list, ListStart) ||
        !zox_has(list, ListVisible))
    {
        return 0;
    }
    const int max_start = get_scrollview_max_start(world, list);
    if (start < 0) {
        start = 0;
    } else if (start > max_start) {
        start = max_start;
    }
    if (zox_getv(list, ListStart) == start) {
        return 0;
    }
    zox_setv(list, ListStart, (byte) start);
    zox_add(list, ListPositionDirty);
    return 1;
}

// scrolls the scrollview's list to start (clamped), returns 1 if it moved
//  - move_handle: also moves the scrollbar handle to match (not while it's being dragged)
byte set_scrollview_start(
    ecs* world,
    entity scrollview,
    int start,
    byte move_handle)
{
    const entity list = zox_get_link(world, scrollview, ListUILink);
    if (!set_list_start(world, list, start)) {
        return 0;
    }
    if (move_handle) {
        // the start clamped as set_list_start did (ListStart's set is deferred, read back it's the old one)
        const int max_start = get_scrollview_max_start(world, list);
        set_scrollbar_handle_to_start(world, scrollview, start < 0 ? 0 : (start > max_start ? max_start : start), max_start);
    }
    return 1;
}

// scrolls a list so its element (a child, by ChildIndex) shows, i.e. navigated to below its shown ones; a 2D list's
//  scrollbar handle with it, a 3D list's too. Returns 1 if it moved
byte scroll_list_to_child(
    ecs* world,
    entity element)
{
    const entity list = zox_get_parent(world, element);
    if (!zox_valid(list) ||
        !zox_has(element, ChildIndex) ||
        !zox_has(list, ListStart) ||
        !zox_has(list, ListVisible))
    {
        return 0;
    }
    const int index = (int) zox_getv(element, ChildIndex);
    const int start = zox_getv(list, ListStart);
    const int visible = zox_getv(list, ListVisible);
    if (visible <= 0) {
        return 0;
    }
    int new_start = start;
    if (index < start) {
        new_start = index;
    } else if (index >= start + visible) {
        new_start = index - visible + 1;
    }
    if (new_start == start) {
        return 0;
    }
    const entity scrollview = zox_get_link(world, list, ScrollviewLink);   // a link (a pair): zox_has doesn't see it
    if (zox_valid(scrollview)) {
        return set_scrollview_start(world, scrollview, new_start, 1);
    }
    return set_list_start(world, list, new_start);
}

// a scroll view's list is its viewport (like Unity's): the area between its scrollbars, its Mask there
//  - size: the scroll view's; right: the vertical bar's width while it shows, bottom: the horizontal bar's height
static inline void scrollview_fit_list_size(ecs* world, entity list, int2 size, int right, int bottom) {
    zox_add(list, Mask);
    zox_setv(list, LayoutSize, ((int2) { int_max(1, size.x - right), int_max(1, size.y - bottom) }));
    zox_setv(list, LayoutPosition, ((int2) { -right / 2, bottom / 2 }));
    zox_add(list, LayoutSizeDirty);
}

// fits it unless it already fits (no relayout then)
static inline void scrollview_fit_list_bars(ecs* world, entity scrollview, entity list, int right, int bottom) {
    const int2 size = zox_getv(scrollview, LayoutSize);
    const int2 list_size = { int_max(1, size.x - right), int_max(1, size.y - bottom) };
    if (zox_has(list, Mask) && int2_equals(zox_getv(list, LayoutSize), list_size) &&
        int2_equals(zox_getv(list, LayoutPosition), ((int2) { -right / 2, bottom / 2 }))) {
        return;
    }
    scrollview_fit_list_size(world, list, size, right, bottom);
}

// a shown bar's thickness: the vertical one's width, the horizontal one's height (0 while hidden or none)
static inline int scrollview_bar_size(ecs* world, entity scrollview, byte horizontal) {
    iter it2 = zox_children(world, scrollview);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            const entity bar = it2.entities[j];
            if (zox_has(bar, Scrollbar) && zox_has(bar, HorizontalScrollbar) == horizontal &&
                !zox_getv(bar, RenderDisabled)) {
                return horizontal ? zox_getv(bar, LayoutSize).y : zox_getv(bar, LayoutSize).x;
            }
        }
    }
    return 0;
}

// the same from a laid out scroll view: its size and which of its bars show (not in a deferred spawn: the spawn
//  passes its sizes to scrollview_fit_list_size)
void scrollview_fit_list(ecs* world, entity scrollview, entity list) {
    if (zox_valid(scrollview) && zox_valid(list) && zox_has(scrollview, LayoutSize)) {
        scrollview_fit_list_bars(world, scrollview, list, scrollview_bar_size(world, scrollview, 0),
            scrollview_bar_size(world, scrollview, 1));
    }
}
