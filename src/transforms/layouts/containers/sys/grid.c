
zox_sys2(GridSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LayoutSize);
    zox_sys_in(GridDirty);
    zox_sys_in(GridSize);
    zox_sys_in(GridPadding);
    zox_sys_in(GridMargins);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(LayoutSize, size);
        zox_sys_i(GridDirty, dirty);
        zox_sys_i(GridSize, grid_size);
        zox_sys_i(GridPadding, padding);
        zox_sys_i(GridMargins, margins);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        entity first_child = zox_get_child_by_id(
            world,
            e,
            zox_id(LayoutSize));
        if (!first_child) {
            continue;
        }
        int2 frame_size = zox_getv(first_child, LayoutSize);
        int2 start_pos = (int2) {
            - size->value.x / 2 + margins->value.x,
            size->value.y / 2 - margins->value.y
        };
        int2 pos = start_pos;
        int k = 0;
        iter it2 = zox_children(world, e);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                if (k >= grid_size->value.x * grid_size->value.y) {
                    break;
                }
                entity child = it2.entities[j];
                int x = k % grid_size->value.x;
                int y = k / grid_size->value.x;
                k++;
                if (x == 0) {
                    pos.x = start_pos.x;
                    if (y == 0) {
                        pos.y -= frame_size.y / 2;
                    } else {
                        pos.y -= frame_size.y;
                    }
                }

                if (!zox_valid(child) ||
                    !zox_has(child, LayoutPositionDirty))
                {
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
            if (k >= grid_size->value.x * grid_size->value.y) {
                break;
            }
        }
    }
} zox_sys_end(GridSystem);

/*zox_sys2(GridSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LayoutSize);
    zox_sys_in(GridDirty);
    zox_sys_in(GridSize);
    zox_sys_in(GridPadding);
    zox_sys_in(GridMargins);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(LayoutSize, size);
        zox_sys_i(GridDirty, dirty);
        zox_sys_i(GridSize, grid_size);
        zox_sys_i(GridPadding, padding);
        zox_sys_i(GridMargins, margins);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        entity children[layouts2_children_capacity];
        uint children_length = zox_get_children(world, e, children, layouts2_children_capacity);
        if (!children_length) {
            continue;
        }
        entity first_child = children[0];
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
                if (k >= children_length) {
                    break;
                }
                entity child = children[k++];

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
} zox_sys_end(GridSystem);*/


