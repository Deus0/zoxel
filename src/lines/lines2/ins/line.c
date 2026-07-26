entity spawn_line2D(ecs *world, float2 pointA, float2 pointB, float thickness, double life_time) {
    entity prefab = life_time ? prefab_temporary_line2D : prefab_line2D;
    zox_instance(prefab);
    // zox_name("line2D") // disabled as crashes if spawning t too many
    zox_set(e, LinePoints2, { pointA, pointB }); // { pointA.x, pointA.y, pointB.x, pointB.y } });
    zox_set(e, LineThickness, { thickness });
    if (life_time) {
        zox_set(e, DestroyInTime, { life_time });
    }
    return e;
}

entity spawn_line2D_colored(ecs *world, float2 pointA, float2 pointB, float thickness, double life_time, color_rgb line_color) {
    entity e = spawn_line2D(world, pointA, pointB, thickness, life_time);
    zox_set(e, Color, { color_rgb_to_color(line_color) });
    return e;
}
