entity zox_get_root_canvas(ecs *world, entity e) {
    if (!zox_valid(e)) {
        return 0;
    }
    if (zox_has(e, Canvas)) {
        return e;
    }
    if (zox_has(e, CanvasLink)) { // shortcut to canvas
        entity canvas = zox_get_value(e, CanvasLink);
        if (zox_valid(canvas)) {
            return canvas;
        }
    }
    return zox_get_root_canvas(world, zox_get_parent(world, e));
}

zox_sys2(CanvasLinkSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(CanvasLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(CanvasLink, canvas);
        if (!zox_valid(canvas->value)) {
            canvas->value = zox_get_root_canvas(world, e);
        }
    }
} zox_sys_end(CanvasLinkSystem);
