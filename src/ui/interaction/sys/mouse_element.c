// element will follow mouse position
//  -> linked to MouseTextureSystem
zox_sys2(MouseElementSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ZeviceLink);
    zox_sys_in(Anchor);
    zox_sys_out(LayoutPosition);
    zox_sys_out(LayoutPositionDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ZeviceLink, zevice);
        zox_sys_i(Anchor, anchor);
        zox_sys_o(LayoutPosition, position);
        zox_sys_o(LayoutPositionDirty, dirty);
        if (!zox_valid(zevice->value)) {
            if (zox_has(e, DeviceLink)) {
                entity device = zox_getv(e, DeviceLink);
                entity mouse_pointer = zox_get_child_by_id(
                    world,
                    device,
                    zox_id(ZevicePointer));
                zox_setv(e, ZeviceLink, mouse_pointer);
            }
            continue;
        }
        if (!zox_has(zevice->value, ZevicePointerPosition)) {
            zox_loge("Pointer Invalid: %s", zox_get_name(zevice->value));
            continue;
        }
        if (zox_getv(zevice->value, ZeviceDisabled)) {
            continue;
        }
        entity canvas = zox_get_parent_by_id(
            world,
            e,
            zox_id(Canvas));
        if (!zox_valid(canvas)) {
            zox_loge("Invalid Canvas for Layout %s", zox_get_name(e));
            continue;
        }
        int2 canvas_size = zox_getv(canvas, LayoutSize);
        int2 output = zox_getv(zevice->value, ZevicePointerPosition);
        output.x -= anchor->value.x * canvas_size.x;
        output.y -= anchor->value.y * canvas_size.y;
        if (!int2_equals(position->value, output)) {
            position->value = output;
            dirty->value = zox_dirty_trigger;
        }
    }
} zox_sys_end(MouseElementSystem);
