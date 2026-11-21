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
        entity children[layouts2_children_capacity];
        const uint children_count = get_list_children_ordered(world, e, children, layouts2_children_capacity);
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
