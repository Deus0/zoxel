void GridSystem(iter *it) {

    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GridDirty);
    zox_sys_in(Children);
    zox_sys_in(LayoutSize);
    zox_sys_in(GridSize);
    zox_sys_in(GridPadding);
    zox_sys_in(GridMargins);

    for (int i = 0; i < it->count; i++) {

        zox_sys_i(GridDirty, dirty);
        zox_sys_i(Children, children);
        zox_sys_i(LayoutSize, size);
        zox_sys_i(GridSize, grid_size);
        zox_sys_i(GridPadding, padding);
        zox_sys_i(GridMargins, margins);

        if (dirty->value != zox_dirty_active || !children->length) {
            continue;
        }

        const entity first_child = children->value[0];
        zox_geter_value(first_child, LayoutSize, int2, frame_size);

        // calculate total size first: ListUIMax - center it?

        int2 start_pos = (int2) {
            - size->value.x / 2 + margins->value.x,
            size->value.y / 2 - margins->value.y
        };
        int2 pos = start_pos;
        int k = 0;

        for (int y = 0; y < grid_size->value.y; y++) {

            pos.x = start_pos.x;
            if (y == 0) {
                pos.y -= frame_size.y / 2;
            } else {
                pos.y -= frame_size.y;
            }

            for (int x = 0; x < grid_size->value.x; x++) {
                if (k >= children->length) break;

                const entity child = children->value[k++];

                if (!zox_valid(child) || !zox_has(child, LayoutPositionDirty)) {
                    continue;
                }

                zox_muter(child, LayoutPosition, position);
                zox_muter(child, LayoutPositionDirty, position_dirty);

                if (x == 0) {
                    pos.x += frame_size.x / 2;
                } else {
                    pos.x += frame_size.x;
                }

                position->value = pos;
                position_dirty->value = zox_dirty_trigger;

                pos.x += padding->value.x;
            }

            pos.y -= padding->value.y;
        }
    }
} zoxd_system2(GridSystem);