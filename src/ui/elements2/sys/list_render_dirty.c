// if outside list visible indexes, hide, otherwise show
// This sets visibility of the list recursively
zox_sys2(ListRenderDirtySystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ListStart);
    zox_sys_in(ListVisible);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ListStart, start);
        zox_sys_i(ListVisible, visible);
        entity children[layouts2_children_capacity];
        const uint children_count = get_list_children_ordered(world, e, children, layouts2_children_capacity);
        for (uint k = 0; k < children_count; k++) {
            const byte is_visible = k >= start->value && k < start->value + visible->value;
            set_children_render_disabled(
                world,
                children[k],
                !is_visible);
        }
    }
} zox_sys_end(ListRenderDirtySystem);
