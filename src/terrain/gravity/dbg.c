// debug: the fields round the camera, a cube a node that doesn't pull down (the gizmos' Gravity button), in the
//  camera's chunk and a chunk round it (RenderDistance 1 or less), coloured by where it pulls
byte zox_debug_gravity = 0;

static inline color_rgb gravity_debug_color(byte value) {
    const color_rgb colors[7] = { { 255, 255, 255 }, { 255, 90, 90 }, { 255, 160, 90 }, { 90, 255, 90 },
        { 190, 90, 255 }, { 90, 140, 255 }, { 90, 255, 255 } };
    return colors[value <= 6 ? value : 0];
}

void gravity_debug_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Position3D);
    zox_sys_in(BlockScale);
    zox_sys_in(RenderDistance);
    zox_sys_in(NodeDepth);
    zox_sys_in(GravityOctree);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Position3D, position);
        zox_sys_i(BlockScale, scale);
        zox_sys_i(RenderDistance, distance);
        zox_sys_i(NodeDepth, depth);
        zox_sys_i(GravityOctree, field);
        if (distance->value > 1) {
            continue;
        }
        const debug_octree_style style = { sizeof(GravityOctree), octree_type_offset_GravityOctree, 0, 0, depth->value,
            gravity_default, 0, gravity_debug_color };
        debug_octree(world, &style, field, NULL, position->value, scale->value * octree_size(depth->value));
    }
    zox_sys_on_end();
} zoxd_system(gravity_debug_system);

void zox_dbg_toggle_gravity_debug(ecs* world, ClickEventData data) {
    zox_debug_gravity = !zox_debug_gravity;
    zox_set_enabled(gravity_debug_system, zox_debug_gravity);
    zox_log("+ Debug Gravity [%s]", zox_debug_gravity ? "on" : "off");
}

void zoxd_systems_gravity_debug(ecs* world) {
    zox_system_1(
        gravity_debug_system,
        zoxp_spawn,
        [in] transforms3.Position3D,
        [in] blocks.BlockScale,
        [in] rendering.RenderDistance,
        [in] chunks.NodeDepth,
        [in] gravities.GravityOctree,
        [none] chunks.Chunk,
    );
    zox_set_enabled(gravity_debug_system, zox_debug_gravity);
}
