/*zox_sys2(CanvasLinkSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(CanvasLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(CanvasLink, canvas);
        if (!zox_valid(canvas->value)) {
            canvas->value = zox_get_parent_by_id(world, e, zox_id(Canvas));
        }
    }
} zox_sys_end(CanvasLinkSystem);*/
