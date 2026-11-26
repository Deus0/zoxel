zox_tag(VirtualList);
zox_tag(ListSlot);
zox_tag(RowTargetLink);
zox_tag(ListStartLink);
zoxc_int(ListScrollRows);
zoxc_float(ListScrollFraction);

static void virtual_list_row_unlinked(ecs* world, entity row, entity target);

static void zox_components_virtual_list(ecs* world) {
    zoxd_tag(VirtualList);
    zoxd_tag(ListSlot);
    zoxd_nf_link(RowTargetLink);
    zoxd_nf_link(ListStartLink);
    zoxd_int(ListScrollRows);
    zoxd_float(ListScrollFraction);
    zox_on_unlink(virtual_list_row_unlinked, RowTargetLink);
}
