entity prefab_line2D;
extern entity prefab_temporary_line2D;

entity spawn_prefab_line2D(ecs *world) {
    zox_prefab();
    zox_prefab_name("line2D");
    zox_add_tag(e, Line2D);
    zox_prefab_set(e, LineData2D, {{ 0, 0, 0, 0 }});
    zox_prefab_set(e, LineThickness, { 1 });
    zox_prefab_set(e, Color, {{ 0, 255, 255, 255 }});
    zox_prefab_set(e, Layer2D, { 0 });
    prefab_line2D = e;
    return e;
}

entity spawn_line2D(
    ecs *world,
    const float2 pointA,
    const float2 pointB,
    const float thickness,
    const double life_time
) {
    entity prefab = life_time ? prefab_temporary_line2D : prefab_line2D;
    zox_instance(prefab);
    // zox_name("line2D") // disabled as crashes if spawning t too many
    zox_set(e, LineData2D, { { pointA.x, pointA.y, pointB.x, pointB.y } });
    zox_set(e, LineThickness, { thickness });
    if (life_time) {
        zox_set(e, DestroyInTime, { life_time });
    }
    return e;
}

entity spawn_line2D_colored(
    ecs *world,
    const float2 pointA,
    const float2 pointB,
    const float thickness,
    const double life_time,
    const color_rgb line_color
) {
    const entity e = spawn_line2D(world, pointA, pointB, thickness, life_time);
    zox_set(e, Color, { color_rgb_to_color(line_color) });
    return e;
}