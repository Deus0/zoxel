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
    zox_add(handle, LayoutPositionDirty);
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
//  scrollbar handle with it, a 3D list (spawn_list3) has none. Returns 1 if it moved
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
