// The overflow range is in scrollview pixels, before applying the list's current offset.
static void update_horizontal_scrollbar(ecs* world, entity scrollview, entity list,
    const entity* children, uint count, byte hidden)
{
    const entity bar = zox_get_child_by_id(world, scrollview, zox_id(HorizontalScrollbar));
    if (!zox_valid(bar)) {
        return;
    }
    const entity handle = zox_get_child_by_id(world, bar, zox_id(ScrollbarHandle));
    if (!zox_valid(handle)) {
        return;
    }
    const int width = zox_getv(scrollview, LayoutSize).x;
    const int list_width = zox_getv(list, LayoutSize).x;
    const int centre = zox_getv(list, LayoutPosition).x +
        (int) (zox_getv(list, Anchor).x * width - width * 0.5f);
    const byte alignment = zox_getv(list, ListAlignment);
    const int margin = zox_getv(list, ListMargins).x;
    const byte has_offset = zox_has(list, ListScrollX);
    int left = -width / 2, right = left + width;
    for (uint i = 0; i < count; i++) {
        const entity row = children[i];
        if (!zox_has(row, LayoutSize) || zox_has(row, ListHidden)) {
            continue;
        }
        const int row_width = zox_getv(row, LayoutSize).x;
        int x = zox_getv(row, LayoutPosition).x;
        if (alignment == zox_alignment_left) {
            x = margin - list_width / 2 + row_width / 2;
        } else if (alignment == zox_alignment_right) {
            x = list_width / 2 - margin - row_width / 2;
        }
        if (zox_has(row, ListIndent) && alignment != zox_alignment_centre) {
            x += zox_getv(row, ListIndent) * (byte) (list_width / 12);
        }
        const int edge = centre + x - row_width / 2;
        left = int_min(left, edge);
        right = int_max(right, edge + row_width);
    }
    const int low = left + width / 2;
    const int high = right + width / 2 - width;
    const int range = high - low;
    const byte disabled = hidden || !range || width <= 1;
    zox_setv(bar, RenderDisabled, disabled);
    zox_setv(handle, RenderDisabled, disabled);
    const int offset = range ? (has_offset ? int_max(low, int_min(high, zox_getv(list, ListScrollX))) : low) : 0;
    if (!has_offset || zox_getv(list, ListScrollX) != offset) {
        zox_setv(list, ListScrollX, offset);
    }
    int2 bar_size = zox_getv(bar, LayoutSize);
    if (bar_size.x != width) {
        bar_size.x = width;
        zox_setv(bar, LayoutSize, bar_size);
        zox_add(bar, LayoutSizeDirty);
    }
    zox_setv(handle, SlideBounds, ((float2) { low, high }));
    const int length = range && width > 1 ? int_max(1, (int) ((float) width * width / (width + range))) : width;
    layout_scrollbar_handle(world, handle, bar_size, length, range ? (offset - low) / (float) range : 0, 1);
    // the bar shows or hides: the viewport follows (its RenderDisabled set above is deferred: use disabled)
    scrollview_fit_list_bars(world, scrollview, list, scrollview_bar_size(world, scrollview, 0),
        disabled ? 0 : bar_size.y);
}

// if outside list visible indexes, hide, otherwise show (all hidden while its scrollview is)
// This sets visibility of the list recursively
void list_render_dirty_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ListStart);
    zox_sys_in(ListVisible);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ListStart, start);
        zox_sys_i(ListVisible, visible);
        // a hidden scrollview hides its list (a window's tab not shown, ui/windows/ins/tabs.c)
        const entity scrollview = zox_get_link(world, e, ScrollviewLink);
        const byte hidden = zox_valid(scrollview) && zox_has(scrollview, RenderDisabled) && zox_getv(scrollview, RenderDisabled);
        const byte virtual = zox_has(e, VirtualList);
        entity children[layouts2_children_capacity + 1];
        const uint children_count = get_list_children_ordered(world, e, children, layouts2_children_capacity + virtual);
        if (zox_valid(scrollview)) {
            update_horizontal_scrollbar(world, scrollview, e, children, children_count, hidden);
        }
        if (virtual) {
            continue;
        }
        for (uint k = 0; k < children_count; k++) {
            const byte is_visible = !hidden && k >= start->value && k < start->value + visible->value;
            set_children_render_disabled(
                world,
                children[k],
                !is_visible);
        }
    }
    zox_sys_on_end();
} zoxd_system(list_render_dirty_system);
