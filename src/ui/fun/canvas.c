// gets an entities canvas root by climbing the ranks
// using this for render culling
entity zox_get_root_canvas_camera(ecs *world, entity e) {
    entity canvas = zox_get_parent_by_id(world, e, zox_id(Canvas));
    return (zox_valid(canvas) && zox_has(canvas, CameraLink)) ? zox_gett_value(canvas, CameraLink) : 0;
}

