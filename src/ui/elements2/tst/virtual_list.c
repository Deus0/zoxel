static byte zox_tst_virtual_list_ready(ecs* world) {
    static entity deleted_slot;
    if (deleted_slot) {
        zox_tst_waiting = "the deleted target's slot to hide";
        if (!zox_getv(deleted_slot, RenderDisabled)) {
            return 0;
        }
        zox_tst_check(!zox_get_link(world, deleted_slot, RowTargetLink), "deleted target left a link");
        return 1;
    }
    const entity canvas = zox_tst_ui_fixture_canvas(world);
    if (!canvas) {
        return 0;
    }
    const entity root = zox_new();
    zox_add_ordered_children(root);
    entity first = 0, last = 0;
    for (int i = 0; i < 5000; i++) {
        last = zox_new();
        zox_set_parent(world, last, root);
        if (!first) {
            first = last;
        }
    }
    const int2 size = { 128, 160 };
    const entity view = spawn_scrollview(world, canvas, int2_zero, size, float2_half, 10, 10, 5000, 1);
    const entity list = spawn_list(world, (LayoutParentData) { .e = canvas }, (LayoutParentData) { .e = view },
        (ElementSpawnData) { .prefab = prefab_list, .anchor = float2_half, .size = size },
        (SpawnList) { .visible_count = 10 }, zox_alignment_left, NULL);
    zox_add(list, VirtualList);
    zox_set_link(world, list, ScrollviewLink, view);
    zox_set_link(world, view, ListUILink, list);
    scrollview_fit_list(world, view, list);
    VirtualListPage page = { 0 };
    virtual_list_prepare_slots(world, list, &page);
    for (int i = 0; i < page.capacity; i++) {
        page.slots[i] = spawn_uic(world, prefab_button, list, float2_half, int2_zero,
            (int2) { 100, 16 }, (int2) { 100, 16 }, color_white, color_white);
        zox_setv(page.slots[i], ChildIndex, i);
        zox_add(page.slots[i], ListSlot);
    }
    const ordered_children children = zox_get_ordered_children(world, root);
    for (int i = 0; i < page.capacity; i++) {
        virtual_list_bind(world, &page, children.value[i]);
    }
    virtual_list_finish(world, list, &page, first, children.length, 0);
    zox_tst_check(zox_get_children_count(world, list) == 11, "expected 11 slots");
    zox_tst_check(zox_get_link(world, page.slots[0], RowTargetLink) == first, "first target missing");
    scroll_virtual_list_fraction(world, list, 1);
    const int start = virtual_list_start(world, list, children.length, 0);
    page.filled = 0;
    for (int i = start; i < children.length; i++) {
        virtual_list_bind(world, &page, children.value[i]);
    }
    virtual_list_finish(world, list, &page, children.value[start], children.length, start);
    const entity handle = get_scrollview_handle(world, view);
    zox_tst_check(zox_get_link(world, page.slots[9], RowTargetLink) == last, "last target missing");
    zox_tst_check(!zox_get_link(world, page.slots[10], RowTargetLink) && zox_getv(page.slots[10], RenderDisabled),
        "spare slot is visible at the end");
    zox_tst_check(zox_getv(handle, LayoutSize).y <= 2 &&
        zox_getv(handle, LayoutPosition).y == zox_getv(handle, LayoutConstraints).z, "handle is not small and at bottom");
    deleted_slot = page.slots[9];
    zox_delete(last);
    return 0;
}
