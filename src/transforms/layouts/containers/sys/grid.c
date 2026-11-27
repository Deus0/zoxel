// places a grid's cells in rows of GridSize.x, from its top left (by its margins), from its first shown row

void grid_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LayoutSize);
    zox_sys_in(GridSize);
    zox_sys_in(GridPadding);
    zox_sys_in(GridMargins);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(LayoutSize, size);
        zox_sys_i(GridSize, grid_size);
        zox_sys_i(GridPadding, padding);
        zox_sys_i(GridMargins, margins);
        entity first_child = zox_get_child_by_id(
            world,
            e,
            zox_id(LayoutSize));
        if (!first_child) {
            continue;
        }
        int2 frame_size = zox_getv(first_child, LayoutSize);
        // scrolled (ListStart, its first shown row): the rows above it sit over its top, its Mask clips them
        const int scrolled = zox_has(e, ListStart) ? zox_getv(e, ListStart) : 0;
        int2 start_pos = (int2) {
            - size->value.x / 2 + margins->value.x,
            size->value.y / 2 - margins->value.y + scrolled * frame_size.y
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

                if (!zox_valid(child)) {
                    continue;
                }
                zox_muter(child, LayoutPosition, position);

                if (x == 0) {
                    pos.x += frame_size.x / 2;
                } else {
                    pos.x += frame_size.x;
                }

                position->value = pos;
                pos.x += padding->value.x;
            }
            if (k >= grid_size->value.x * grid_size->value.y) {
                break;
            }
        }
    }
    zox_sys_on_end();
} zoxd_system(grid_system);
