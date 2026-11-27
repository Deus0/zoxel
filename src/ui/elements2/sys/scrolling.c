// Dragging a scrollbar's handle scrolls its list, by how far the handle is along the scrollbar
void scrollbar_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DraggableState);
    zox_sys_in(LayoutPosition);
    zox_sys_in(LayoutSize);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(DraggableState, state);
        zox_sys_i(LayoutPosition, position);
        zox_sys_i(LayoutSize, size);
        if (!state->value) {
            continue;
        }
        const entity scrollview = zox_get_link(world, e, ScrollviewLink);
        const entity scrollbar = zox_get_parent(world, e);
        if (!zox_valid(scrollview) || !zox_valid(scrollbar) || !zox_has(scrollbar, LayoutSize)) {
            zox_loge("Scrollbar handle [%s] has no scrollview or scrollbar", zox_getn(e));
            continue;
        }
        const entity list = zox_get_link(world, scrollview, ListUILink);
        if (!zox_valid(list)) {
            continue;
        }
        if (zox_has(scrollbar, HorizontalScrollbar)) {
            const int4 constraints = zox_getv(e, LayoutConstraints);
            const int travel = constraints.y - constraints.x;
            if (travel > 0 && !zox_getv(e, RenderDisabled)) {
                const float2 bounds = zox_getv(e, SlideBounds);
                const float fraction = (position->value.x - constraints.x) / (float) travel;
                const int offset = (int) roundf(bounds.x + fraction * (bounds.y - bounds.x));
                if (zox_getv(list, ListScrollX) != offset) {
                    zox_setv(list, ListScrollX, offset);
                }
            }
            continue;
        }
        if (zox_has(list, VirtualList)) {
            const int4 limits = zox_getv(e, LayoutConstraints);
            if (limits.w > limits.z) {
                scroll_virtual_list_fraction(world, list, (limits.w - position->value.y) / (float) (limits.w - limits.z));
            }
            continue;
        }
        // travel: how far the handle can move, none if the list fits
        const int travel = zox_getv(scrollbar, LayoutSize).y - size->value.y;
        if (travel <= 0) {
            continue;
        }
        // the top of the scrollbar is start 0
        const float percentage = (travel / 2 - position->value.y) / (float) travel;
        const int max_start = get_scrollview_max_start(world, list);
        set_scrollview_start(world, scrollview, (int) round(max_start * percentage), 0);
    }
    zox_sys_on_end();
} zoxd_system(scrollbar_system);

// is e the ancestor, or one of its descendants
static inline byte is_ui_within(
    ecs* world,
    entity e,
    entity ancestor)
{
    for (int depth = 0; zox_valid(e) && depth < 32; depth++) {
        if (e == ancestor) {
            return 1;
        }
        e = zox_get_parent(world, e);
    }
    return 0;
}

// The mouse wheel scrolls the scrollview under the pointer, one element per notch
//  - the topmost shown scrollview the pointer is inside, also between its list elements
//  - unless the pointer is over ui unrelated to it, i.e. another window on top of it
//      (its own elements, and the window body behind it, don't block it)
void scrollview_wheel_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_query();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ZeviceWheel);
    zox_sys_in(ZevicePointerPosition);
    zox_sys_in(Target);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ZeviceWheel, wheel);
        zox_sys_i(ZevicePointerPosition, pointer);
        zox_sys_i(Target, target);
        if (!wheel->value.y) {
            continue;
        }
        const entity device = zox_get_parent(world, e);
        const entity player = zox_valid(device) ? zox_get_parent(world, device) : 0;
        const entity canvas = zox_valid(player) ? zox_get_link(world, player, CanvasLink) : 0;
        CanvasViewport viewport;
        if (!get_canvas_viewport(world, canvas, &viewport)) {
            continue;
        }
        entity scrollview = 0;
        int top_layer = -1;
        zox_sys_query_begin();
        while (zox_sys_query_loop()) {
            zox_sys_begin_2();
            zox_sys_in_2(Position2);
            zox_sys_in_2(LayoutSize);
            for (int j = 0; j < it2.count; j++) {
                zox_sys_i_2(Position2, position2);
                zox_sys_i_2(LayoutSize, size2);
                const entity e2 = it2.entities[j];
                // a hidden one (a window's other tabs, all in the same place: the wheel took a hidden tab's)
                if (zox_has(e2, RenderDisabled) && zox_getv(e2, RenderDisabled)) {
                    continue;
                }
                if (zox_get_parent_by_id(world, e2, zox_id(Canvas)) != canvas) {
                    continue;
                }
                const int4 bounds = get_element_pointer_bounds(world, &viewport, e2, position2->value, size2->value);
                if (!is_in_pointer_bounds(bounds, pointer->value)) {
                    continue;
                }
                const entity list = zox_get_link(world, e2, ListUILink);
                const int layer = zox_valid(list) && zox_has(list, Layer) ? zox_getv(list, Layer) : 0;
                if (layer > top_layer) {
                    top_layer = layer;
                    scrollview = e2;
                }
            }
        }
        zox_sys_query_end();
        if (!scrollview) {
            continue;
        }
        if (zox_valid(target->value) &&
            !is_ui_within(world, target->value, scrollview) &&
            !is_ui_within(world, scrollview, target->value))
        {
            continue;
        }
        const entity list = zox_get_link(world, scrollview, ListUILink);
        if (!zox_valid(list)) {
            continue;
        }
        if (zox_has(list, VirtualList)) {
            scroll_virtual_list_rows(world, list, -wheel->value.y);
            continue;
        }
        // wheel up (positive) scrolls back to the top
        const int start = zox_getv(list, ListStart);
        set_scrollview_start(world, scrollview, start - wheel->value.y, 1);
    }
    zox_sys_on_end();
} zoxd_system(scrollview_wheel_system);

// Pressing a scrollbar's track (not its handle) moves the handle under the pointer, then drags it from there
//  (scrollbar_system scrolls the list to it)
void scrollbar_track_press_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ZevicePointer);
    zox_sys_in(ZevicePointerPosition);
    zox_sys_in(Target);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ZevicePointer, click);
        zox_sys_i(ZevicePointerPosition, pointer);
        zox_sys_i(Target, target);
        const entity bar = target->value;
        if (!devices_get_pressed_this_frame(click->value) || !zox_valid(bar) ||
            !(zox_has(bar, Scrollbar) || zox_has(bar, HorizontalScrollbar)) || !zox_has(bar, Position2)) {
            continue;
        }
        const entity handle = zox_get_child_by_id(world, bar, zox_id(ScrollbarHandle));
        const entity device = zox_get_parent(world, e);
        const entity player = zox_valid(device) ? zox_get_parent(world, device) : 0;
        CanvasViewport viewport;
        if (!zox_valid(handle) || zox_getv(handle, RenderDisabled) || !zox_valid(player) ||
            !get_canvas_viewport(world, zox_get_parent_by_id(world, bar, zox_id(Canvas)), &viewport)) {
            continue;
        }
        const int4 bounds = get_element_pointer_bounds(world, &viewport, bar, zox_getv(bar, Position2), zox_getv(bar, LayoutSize));
        const int4 limits = zox_getv(handle, LayoutConstraints);
        int2 position = zox_getv(handle, LayoutPosition);
        if (zox_has(bar, HorizontalScrollbar)) {
            position.x = int_clamp(pointer->value.x - (bounds.x + bounds.y) / 2, limits.x, limits.y);
        } else {
            position.y = int_clamp(pointer->value.y - (bounds.z + bounds.w) / 2, limits.z, limits.w);
        }
        zox_setv(handle, LayoutPosition, position);
        zox_set_link(world, handle, ZeviceLink, e);
        set_element_dragged(world, player, handle, zox_drag_mode_mouse);
    }
    zox_sys_on_end();
} zoxd_system(scrollbar_track_press_system);
