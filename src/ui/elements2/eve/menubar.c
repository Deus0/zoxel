static void menubar_layout_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        const entity menu = it->entities[i];
        const entity bar = zox_get_parent(world, menu);
        if (!zox_has(bar, Menubar)) {
            continue;
        }
        zox_set_link(world, menu, ItemListCanvas, zox_get_link(world, bar, ItemListCanvas));
        int x = 0;
        const ordered_children children = zox_get_ordered_children(world, bar);
        for (int j = 0; j < children.length; j++) {
            const entity sibling = children.value[j];
            if (zox_has(sibling, Menu)) {
                const int width = zox_getv(sibling, LayoutSize).x;
                zox_setv(sibling, LayoutPosition, ((int2) { x + width / 2, 0 }));
                x += width;
            }
        }
    }
}

static void menubar_select_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        const entity menu = it->entities[i];
        const entity bar = zox_get_parent(world, menu);
        const entity canvas = zox_get_link(world, menu, ItemListCanvas);
        const entity previous = zox_get_link(world, canvas, OpenItemList);
        if (previous && previous != menu && zox_has(bar, Menubar) && zox_get_parent(world, previous) == bar) {
            item_list_open(world, menu);
        }
    }
}
