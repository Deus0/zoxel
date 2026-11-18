// A 3D window's tooltip (spawn_window on 3D huds) under the ui it's showing: centred below it, a little gap, a
//  little nearer the eye, turned as its window is
//  - its window's metres a pixel: the canvas3 is Scale1 metres high, LayoutSize pixels
//  - its own transform (DisableParentTransform): placed here, its glyphs on it by the hierarchy (set_position_rotation_
//      scale_recursive keeps its transform, places its children from it)
#define tooltip3_gap 2            // canvas pixels between the ui and its tooltip
#define tooltip3_nearer 0.01f     // metres towards the eye, over the window (4mm had its top half hidden behind its window's frame at low resolution)

void tooltip3_follow_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LayoutSize);
    zox_sys_out(Position3D);
    zox_sys_out(Rotation3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(LayoutSize, size);
        zox_sys_o(Position3D, position3);
        zox_sys_o(Rotation3D, rotation3);
        const entity ui = zox_get_link(world, e, TooltipLink);
        const entity window = zox_get_parent(world, e);
        if (!zox_valid(ui) || !zox_valid(window) || !zox_has(window, Canvas3) ||
            !zox_has(ui, Position3D) || !zox_has(ui, LayoutSize)) {
            continue;
        }
        const int2 window_size = zox_getv(window, LayoutSize);
        const float metres = window_size.y > 0 ? zox_getv(window, Scale1) / window_size.y : 0;
        const float4 rotation = zox_getv(window, Rotation3D);
        const float below = (zox_getv(ui, LayoutSize).y + size->value.y) * 0.5f + tooltip3_gap;
        const float3 position = float3_add(zox_getv(ui, Position3D),
            float4_rotate_float3(rotation, (float3) { 0, -below * metres, tooltip3_nearer }));
        position3->value = position;
        rotation3->value = rotation;
    }
    zox_sys_on_end();
} zoxd_system(tooltip3_follow_system);
