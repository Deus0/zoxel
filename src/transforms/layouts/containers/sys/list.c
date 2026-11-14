// the height a list's shown children need: theirs, a padding between, its margins round them
static inline int list_fit_height(ecs* world, const entity* children, uint count, byte start, int2 padding, int2 margins) {
    int height = margins.y * 2, shown = 0;
    for (uint k = start; k < count; k++) {
        if (zox_valid(children[k]) && zox_has(children[k], LayoutSize) && !zox_has(children[k], ListHidden)) {
            height += zox_getv(children[k], LayoutSize).y;
            shown++;
        }
    }
    return height + (shown > 1 ? (shown - 1) * padding.y : 0);
}

// an element grown by delta in height, its anchored edge kept (Anchor y: 0 its bottom, 1 its top, else its middle)
static inline void list_grow(ecs* world, entity e, int delta) {
    zox_muter(e, LayoutSize, size);
    size->value.y += delta;
    zox_add(e, LayoutSizeDirty);
    const float anchor = zox_has(e, Anchor) ? zox_getv(e, Anchor).y : 0.5f;
    if (anchor != 0.5f) {
        zox_muter(e, LayoutPosition, position);
        position->value.y += anchor < 0.5f ? delta / 2 : -delta / 2;
        zox_add(e, LayoutPositionDirty);
    }
}

// NOTE: Repositions list elements (in ChildIndex order) when ListPositionDirty is active
//  - hidden children (ListHidden) are skipped, their gaps closed
//  - a ListFit list fits its height to its shown children first (ListResizesParent: its parent grows with it)
void list_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(LayoutSize);
    zox_sys_in(ListPadding);
    zox_sys_in(ListMargins);
    zox_sys_in(ListStart);
    zox_sys_in(ListAlignment);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(LayoutSize, size);
        zox_sys_i(ListPadding, padding);
        zox_sys_i(ListMargins, margins);
        zox_sys_i(ListStart, start);
        zox_sys_i(ListAlignment, alignment);
        entity children[layouts2_children_capacity];
        const uint children_count = get_list_children_ordered(world, e, children, layouts2_children_capacity);
        if (zox_has(e, ListFit)) {
            const int2 pad = { padding->value.x, padding->value.y }, margin = { margins->value.x, margins->value.y };
            const int delta = list_fit_height(world, children, children_count, start->value, pad, margin) - size->value.y;
            if (delta) {
                size->value.y += delta;
                zox_add(e, LayoutSizeDirty);
                const float anchor = zox_has(e, Anchor) ? zox_getv(e, Anchor).y : 0.5f;
                if (anchor != 0.5f) {
                    zox_muter(e, LayoutPosition, position);
                    position->value.y += anchor < 0.5f ? delta / 2 : -delta / 2;
                    zox_add(e, LayoutPositionDirty);
                }
                const entity parent = zox_get_parent(world, e);
                if (zox_has(e, ListResizesParent) && zox_valid(parent) && zox_has(parent, LayoutSize)) {
                    list_grow(world, parent, delta);
                }
            }
        }
        byte indent_size = size->value.x / 12;
        int list_position_y = (int)(size->value.y / 2);
        list_position_y -= margins->value.y;
        int previous_height = 0;
        byte first = 1;
        for (uint k = 0; k < children_count; k++) {
            entity e2 = children[k];
            if (!zox_valid(e2) || !zox_has(e2, LayoutPosition) || zox_has(e2, ListHidden)) {
                continue;
            }
            if (k < start->value) {
                continue;
            }
            int2 child_size = zox_getv(e2, LayoutSize);
            if (first) {
                list_position_y -= child_size.y / 2;
                first = 0;
            } else {
                list_position_y -= previous_height / 2;
                list_position_y -= padding->value.y;
                list_position_y -= child_size.y / 2;
            }
            zox_add(e2, LayoutPositionDirty);
            zox_muter(e2, LayoutPosition, position);
            position->value.y = list_position_y;
            if (alignment->value == zox_alignment_left) {
                position->value.x = margins->value.x - size->value.x / 2 + child_size.x / 2;
            } else if (alignment->value == zox_alignment_right) {
                position->value.x = -margins->value.x + size->value.x / 2 - child_size.x / 2;
            }
            if (zox_has(e2, ListIndent)) {
                byte indent = zox_getv(e2, ListIndent);
                position->value.x += indent * indent_size;
            }
            previous_height = child_size.y;
        }
    }
    zox_sys_on_end();
} zoxd_system(list_system);
