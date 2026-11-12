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

// scrolls the list to start (clamped), returns 1 if it moved
//  - move_handle: also moves the scrollbar handle to match (not while it's being dragged)
byte set_scrollview_start(
    ecs* world,
    entity scrollview,
    int start,
    byte move_handle)
{
    const entity list = zox_get_link(world, scrollview, ListUILink);
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
    if (move_handle) {
        set_scrollbar_handle_to_start(world, scrollview, start, max_start);
    }
    return 1;
}
