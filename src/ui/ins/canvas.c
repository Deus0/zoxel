entity spawn_canvas(
    ecs *world,
    entity prefab,
    entity camera,
    int2 size,
    float4 screen_to_canvas,
    entity app)
{
    zox_instance(prefab);
    zox_name("canvas");
    // NOTE: I removed this before, I think it caused a bug...
    zox_set_parent(world, e, app);
    zox_setv(e, LayoutPosition, int2_half(size));
    zox_setv(e, LayoutSize, size);
    zox_setv(e, ScreenToCanvas, screen_to_canvas);
    zox_link(world, e, App, app);
    zox_link(world, app, Canvas, e);
    zox_link(world, e, CameraLink, camera);
    return e;
}
