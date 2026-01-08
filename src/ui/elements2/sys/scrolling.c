// when scroll_front_drags, get window, move elements by amount
//      > e->parent->parent->children[2+] for list elements - if vertical scrollbar

// TODO: ListLink -> Link scrollbar directly to list
void ScrollbarSystem(iter *it) {

    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DraggableState);
    zox_sys_in(LayoutPosition);
    zox_sys_in(LayoutSize);
    zox_sys_in(ParentLink);

    for (int i = 0; i < it->count; i++) {

        zox_sys_i(DraggableState, state);
        zox_sys_i(LayoutPosition, position);
        zox_sys_i(LayoutSize, size);
        zox_sys_i(ParentLink, parent);

        if (!state->value) {
            continue;
        }

        const entity scrollbar = parent->value;
        zox_geter_value(scrollbar, LayoutSize, int2, scrollbarsize);
        if (scrollbarsize.y == size->value.y) {
            continue;
        }
        entity scrollview = zox_get_value(scrollbar, ParentLink);
        zox_geter(scrollview, Children, scrollview_children);

        entity list_ui = scrollview_children->value[1];


        int scrollbar_offset = scrollbarsize.y - size->value.y;
        int anchored_position_y = -position->value.y + scrollbar_offset / 2;
        float scroll_percentage = anchored_position_y / (float) scrollbar_offset;
        // scrollview_spawn - scrollview_shown gives us total_starts

        // zox_geter_value(list_ui, LayoutSize, int2, window_size)
        zox_geter_value(list_ui, ListVisible, byte, visible);
        zox_geter_value(list_ui, ListStart, byte, start);
        zox_geter(list_ui, Children, list_elements);
        byte total_elements = list_elements->length;
        byte total_starts = total_elements - visible;

        byte new_start = (byte) round(total_starts * scroll_percentage);
        if (start != new_start) {
            // zox_log("new scroll position: %i", new_start);
            zox_set(list_ui, ListStart, { new_start });
            zox_set(list_ui, ListDirty, { zox_dirty_trigger });
        }
    }
} zoxd_system2(ScrollbarSystem);

/*const int scrollbar_margins = zox_gett_value(scrollbar, ElementMargins).x;
const int scrollbar_width = scrollsize->value.x;
byte2 button_padding = (byte2) { (int) (scaled_font_size * 0.46f), (int) (scaled_font_size * 0.3f) };
int2 list_margins = (int2) { (int) (scaled_font_size * 0.8f), (int) (scaled_font_size * 0.8f) };
int button_inner_margins = (int) (scaled_font_size * 0.5f);

for (int j = list_start; j < list_elements->length; j++) {
    int normal_j = j - list_start;
    // set list position to scrollview_index_offset
    const entity list_element = list_elements->value[j];
    int k = normal_j - scrollview_index_offset;

    int2 position = (int2) {
        0,
        (int) (window_size.y / 2) - (k + 0.5f) * (scaled_font_size + button_padding.y * 2) - list_margins.y - k * button_inner_margins
    };

    position.x -= (scrollbar_width + scrollbar_margins * 2) / 2;

    LayoutPosition *element_pixel_position = zox_get_mut(list_element, LayoutPosition)
    if (!int2_equals(element_pixel_position->value, position)) {
        element_pixel_position->value = position;
        zox_modified(list_element, LayoutPosition);
    }
    byte render_enabled = normal_j >= scrollview_index_offset && normal_j < scrollview_index_offset + elements_visible;
    set_children_render_disabled(world, list_element, !render_enabled);
}*/