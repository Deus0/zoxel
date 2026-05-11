// gets an entities canvas root by climbing the ranks
// using this for render culling
entity zox_get_root_canvas_camera(ecs *world, entity e) {
    entity canvas = zox_get_parent_by_id(world, e, zox_id(Canvas));
    // entity canvas = zox_get_root_canvas(world, e);
    if (!canvas || !zox_has(canvas, CameraLink)) {
        return 0;
    } else {
        return zox_get_value(canvas, CameraLink);
    }
}

