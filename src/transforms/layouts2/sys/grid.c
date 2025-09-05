void GridSystem(iter *it) {

    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GridDirty);
    zox_sys_in(Children);
    zox_sys_in(LayoutSize);
    zox_sys_in(GridPadding);
    zox_sys_in(GridMargins);

    for (int i = 0; i < it->count; i++) {

        zox_sys_i(GridDirty, dirty);
        zox_sys_i(Children, children);
        zox_sys_i(LayoutSize, size);
        zox_sys_i(GridPadding, padding);
        zox_sys_i(GridMargins, margins);

        if (dirty->value != zox_dirty_active) {
            continue;
        }

        // calculate total size first: ListUIMax - center it?

        int2 pos = (int2) {
            - size->value.x / 2,
            size->value.y / 2
        };
        // (int) (size->value.y / 2);
        // list_position_y -= margins->value.y;

        for (int j = 0; j < children->length; j++) {
            for (int j = 0; j < children->length; j++) {
                const entity child = children->value[j];

                if (!zox_valid(child) || !zox_has(child, LayoutPositionDirty)) {
                    continue;
                }

                zox_geter_value(child, LayoutSize, int2, child_size);
                zox_muter(child, LayoutPosition, position);
                zox_muter(child, LayoutPositionDirty, position_dirty);

                if (j == 0) {
                    pos.x += child_size.x / 2;
                    pos.y -= child_size.y / 2;
                } else {
                    pos.x += child_size.x;
                    pos.y -= child_size.y;
                }

                position->value = pos;
                position_dirty->value = zox_dirty_trigger;

                pos.x += padding->value.x;
                pos.y -= padding->value.y;
            }
        }
    }
} zoxd_system2(GridSystem);