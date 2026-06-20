void prefab_add_cube_lines_shrink(ecs* world, entity e, color c, byte active, float shrink) {
    zox_add_tag(e, CubeLines);
    zox_prefab_set(e, DebugCubeLines, { active });
    zox_prefab_set(e, LineThickness, { 1 });
    zox_prefab_set(e, Color, { c });
    if (shrink > 0) {
        zox_prefab_set(e, DebugCubeShrink, { shrink });
        zox_add_tag(e, DebugCubeCorner);
    }
}

void prefab_add_cube_lines(ecs *world, entity e, color c, byte active) {
    zox_add_tag(e, CubeLines);
    zox_prefab_set(e, DebugCubeLines, { active });
    zox_prefab_set(e, LineThickness, { 1 });
    zox_prefab_set(e, Color, { c });
}

entity spawn_prefab_cube_lines(ecs *world) {
    zox_prefab();
    zox_prefab_name("cube_lines");
    prefab_add_cube_lines(world, e, color_white, 1);
    zox_prefab_set(e, Position3D, { float3_zero });
    zox_prefab_set(e, Rotation3D, { quaternion_identity });
    zox_prefab_set(e, RenderDepth, { 0 });
    zox_prefab_set(e, Bounds3D, { float3_zero });
    zox_prefab_set(e, DestroyInTime, { 0 });
    return e;
}

entity spawn_cube_lines(ecs *world, float3 position, float3 extents, float thickness, color_rgb line_color, double life_time) {
    zox_instance(prefab_cube_lines);
    // zox_name("cube_lines")
    zox_set(e, Position3D, { position });
    zox_set(e, Bounds3D, { extents });
    zox_set(e, LineThickness, { thickness });
    zox_set(e, Color, { color_rgb_to_color(line_color) });
    if (life_time) {
        zox_set(e, DestroyInTime, { life_time });
    }
    return e;
}

entity spawn_cube_lines_rgba(ecs* world, float3 position, float3 extents, float thickness, color fill, double life_time) {
    zox_instance(prefab_cube_lines);
    // zox_name("cube_lines")
    zox_set(e, Position3D, { position });
    zox_set(e, Bounds3D, { extents });
    zox_set(e, LineThickness, { thickness });
    zox_set(e, Color, { fill });
    if (life_time) {
        zox_set(e, DestroyInTime, { life_time });
    }
    return e;
}
