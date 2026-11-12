// element will follow mouse position
//  -> linked to MouseTextureSystem
void MouseElementSystem(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Anchor);
    zox_sys_out(LayoutPosition);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        entity zevice = zox_get_link(world, e, ZeviceLink);
        zox_sys_i(Anchor, anchor);
        zox_sys_o(LayoutPosition, position);
        if (!zox_valid(zevice)) {
            entity device = zox_get_link(world, e, DeviceLink);
            if (zox_valid(device)) {
                entity mouse_pointer = zox_get_child_by_id(
                    world,
                    device,
                    zox_id(ZevicePointer));
                zox_set_link(world, e, ZeviceLink, mouse_pointer);
            }
            continue;
        }
        if (!zox_has(zevice, ZevicePointerPosition)) {
            zox_loge("Pointer Invalid: %s", zox_get_name(zevice));
            continue;
        }
        if (zox_has(zevice, Disabled)) {
            continue;
        }
        entity canvas = zox_get_parent_by_id(
            world,
            e,
            zox_id(Canvas));
        if (!zox_valid(canvas)) {
            zox_loge("[MouseElementSystem] Invalid [canvas]: %s", zox_getn(e));
            continue;
        }
        int2 canvas_size = zox_getv(canvas, LayoutSize);
        int2 output = zox_getv(zevice, ZevicePointerPosition);
        output.x -= anchor->value.x * canvas_size.x;
        output.y -= anchor->value.y * canvas_size.y;
        if (!int2_equals(position->value, output)) {
            position->value = output;
            zox_add(e, LayoutPositionDirty);
        }
    }
    zox_sys_on_end();
} zoxd_system(MouseElementSystem);
