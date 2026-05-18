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
            continue;
        }
        entity canvas = zox_get_parent_by_id(world, e, zox_id(Canvas));
        if (!zox_valid(canvas)) {
            zox_loge("Invalid Canvas for Layout %s", zox_get_name(e));
            continue;
        }
        zox_geter_value(zevice->value, DeviceLink, entity, device);
        if (!zox_valid(device) || zox_gett_value(device, DeviceDisabled)) {
            continue;
        }
        if (!zox_has(zevice->value, ZevicePointerPosition)) {
            zox_log(" > mouse link invalid, needs pointer position: %s\n", zox_get_name(zevice->value))
        }
        zox_geter_value(canvas, LayoutSize, int2, canvas_size);
        zox_geter_value_non_const(zevice->value, ZevicePointerPosition, int2, output);
        output.x -= anchor->value.x * canvas_size.x;
        output.y -= anchor->value.y * canvas_size.y;
        if (!int2_equals(position->value, output)) {
            position->value = output;
            dirty->value = zox_dirty_trigger;
        }
    }
} zox_sys_end(MouseElementSystem);
