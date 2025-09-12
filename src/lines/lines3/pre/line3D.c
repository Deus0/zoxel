entity spawn_prefab_line3D(ecs *world) {
    zox_prefab()
    zox_prefab_name("line3D")
    zox_add_tag(e, Line3D)
    zox_prefab_set(e, LineData3D, { { 0, 0, 0, 0, 0, 0 } })
    zox_prefab_set(e, LineThickness, { 1 })
    zox_prefab_set(e, DestroyInTime, { 0 })
    zox_prefab_set(e, Color, { { 0, 0, 0, 255 } })
    return e;
}

entity spawn_line3D(
    ecs *world,
    const float3 pointA,
    const float3 pointB,
    const float thickness,
    const double lifetime
) {
    entity prefab = lifetime ? prefab_temporary_line3D : prefab_line3D;
    zox_instance(prefab);
    zox_set(e, LineData3D, { { pointA.x, pointA.y, pointA.z, pointB.x, pointB.y, pointB.z } });
    zox_set(e, LineThickness, { thickness });
    if (lifetime) {
        zox_set(e, DestroyInTime, { lifetime });
    }
    return e;
}

entity spawn_line3D_colored(
    ecs *world,
    const float3 a,
    const float3 b,
    const float thickness,
    const double life_time,
    const color_rgb c
) {
    const entity e = spawn_line3D(world, a, b, thickness, life_time);
    zox_set(e, Color, { color_rgb_to_color(c) });
    return e;
}

entity spawn_line3D_colored_alpha(
    ecs *world,
    const float3 pointA,
    const float3 pointB,
    const float thickness,
    const double life_time,
    const color line_color
) {
    const entity e = spawn_line3D(world, pointA, pointB, thickness, life_time);
    zox_set(e, Color, { line_color });
    return e;
}

void render_line3D_thickness(
    ecs *world,
    const float3 a,
    const float3 b,
    const color_rgb line_color,
    const float thickness
) {
    spawn_line3D_colored(world, a, b, thickness, line_frame_timing, line_color);
}

void render_line3D_thickness_alpha(
    ecs *world,
    const float3 a,
    const float3 b,
    const color line_color,
    const float thickness
) {
    spawn_line3D_colored_alpha(
        world,
        a,
        b,
        thickness,
        line_frame_timing,
        line_color
    );
}

void render_line3D(
    ecs *world,
    const float3 a,
    const float3 b,
    const color_rgb line_color
) {
    spawn_line3D_colored(world, a, b, 4, line_frame_timing, line_color);
}

/*void spawn_line3D_square(ecs *world, float3 point, float3 size, float thickness, double life_time) {
    spawn_line3D(world,
        float3_add(point, (float3) { -size.x / 2.0f, -size.y / 2.0f }),
        float3_add(point, (float3) { size.x / 2.0f, -size.y / 2.0f }),
        thickness, life_time);
    spawn_line3D(world,
        float3_add(point, (float3) { size.x / 2.0f, -size.y / 2.0f }),
        float3_add(point, (float3) { size.x / 2.0f, size.y / 2.0f }),
        thickness, life_time);
    spawn_line3D(world,
        float3_add(point, (float3) { size.x / 2.0f, size.y / 2.0f }),
        float3_add(point, (float3) { -size.x / 2.0f, size.y / 2.0f }),
        thickness, life_time);
    spawn_line3D(world,
        float3_add(point, (float3) { -size.x / 2.0f, size.y / 2.0f }),
        float3_add(point, (float3) { -size.x / 2.0f, -size.y / 2.0f }),
        thickness, life_time);
}*/
