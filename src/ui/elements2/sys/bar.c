zox_sys2(ElementbarSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LayoutSize);
    zox_sys_in(BarLevel);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(LayoutSize, parent_size);
        zox_sys_i(BarLevel, percentage);
        entity frontbar = zox_get_child_by_id(world, e, zox_id(BarFront));
        if (!zox_valid(frontbar)) {
            zox_logw("Frontbar not found in [%s]", zox_getn(e));
            continue;
        }
        /*entity children[layouts2_children_capacity];
        uint children_length = zox_get_children(world, e, children, layouts2_children_capacity);
        if (!children_length) {
            continue;
        }
        entity frontbar = children[0];
        if (!zox_valid(frontbar)) {
            continue;
        }*/
#ifdef zox_safety_checks
        if (!zox_has(frontbar, LayoutPosition)) {
            zox_log_error("frontbar missing LayoutPosition");
            continue;
        }
        if (!zox_has(frontbar, LayoutPositionDirty)) {
            zox_log_error("frontbar missing LayoutPositionDirty");
            continue;
        }
#endif
        // # Important: Check if busy still
        if (zox_has(frontbar, Initialize)) {
            continue;
        }
        if (zox_has(frontbar, MeshDirty) && zox_getv(frontbar, MeshDirty)) {
            if (dbg_log >= 2) {
                zox_log("Frontbar [%s] busy", zox_getn(e));
            }
            continue;
        }
        if (zox_has(frontbar, GenerateTexture) && zox_getv(frontbar, GenerateTexture)) {
            if (dbg_log >= 2) {
                zox_log("Frontbar [%s] busy", zox_getn(e));
            }
            continue;
        }
        if (zox_has(frontbar, TextureDirty) && zox_getv(frontbar, TextureDirty)) {
            if (dbg_log >= 2) {
                zox_log("Frontbar [%s] busy", zox_getn(e));
            }
            continue;
        }
        zox_muter(frontbar, LayoutPositionDirty, pdirty);
        zox_muter(frontbar, LayoutSizeDirty, sdirty);
        if (pdirty->value || sdirty->value) {
            if (dbg_log >= 2) {
                zox_log("Frontbar [%s] busy", zox_getn(e));
            }
            continue;
        }
        zox_muter(frontbar, LayoutSize, size);
        // we should also set LayoutSize here and dirty for it
        // float2 scale = elementBarSize->value; scale.x *  * scale.x
        // const float left_offset = - scale.x * (1.0f - percentage) * 0.5f;
        // int2 front_size = parent_size->value; // zox_get_value(e, LayoutSize)
        // calculate margin cause im too lazy to put it here as component yet
        // int offset_x = (front_size.x - (front_size.x)) / 2;
        // float percecentage_2 = ((int) (percentage->value * 100)) / 100.0f; // only update per 100 units
        // int new_size = (int) floor(parent_size->value.x * percecentage_2);
        int new_size = (int) floor(parent_size->value.x * percentage->value);
        if (size->value.x != new_size) {
            size->value.x = new_size;
            // left aligned inside parent
            zox_muter(frontbar, LayoutPosition, position);
            position->value.x = -(parent_size->value.x - new_size) / 2;
            pdirty->value = zox_dirty_trigger;
            sdirty->value = zox_dirty_trigger;
            if (dbg_log) {
                zox_log("Frontbar Dirty [%s] Size [%i] Parent Width [%i] Percentage [%f]", zox_getn(e), new_size, parent_size->value.x, percentage->value);
            }
        } else {
            if (dbg_log >= 2) {
                zox_log("Frontbar Same [%s] Size [%i] Parent Width [%i] Percentage [%f]", zox_getn(e), new_size, parent_size->value.x, percentage->value);
            }
        }
    }
} zox_sys_end(ElementbarSystem);
