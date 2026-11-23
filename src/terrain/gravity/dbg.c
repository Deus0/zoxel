// debug: the fields round the camera, a cube a node that doesn't pull down (the gizmos' Gravity button), in the
//  camera's chunk and a chunk round it (RenderDistance 1 or less), coloured by where it pulls
byte zox_debug_gravity = 0;

static inline color_rgb gravity_debug_color(byte value) {
    const color_rgb colors[7] = { { 255, 255, 255 }, { 255, 90, 90 }, { 255, 160, 90 }, { 90, 255, 90 },
        { 190, 90, 255 }, { 90, 140, 255 }, { 90, 255, 255 } };
    return colors[value <= 6 ? value : 0];
}

static void gravity_debug_node(ecs* world, const GravityOctree* node, float3 position, float scale) {
    const GravityOctree* kids = get_children_GravityOctree(node);
    if (!kids) {
        if (node->value != gravity_default) {
            const float3 center = float3_add(position, float3_single(scale * 0.5f));
            debug_cubec(world, center, float3_single(scale * debug_octree_padding), gravity_debug_color(node->value),
                debug_octree_thickness);
        }
        return;
    }
    const float half = scale * 0.5f;
    for (byte i = 0; i < 8; i++) {
        const byte3 o = octree_positions[i];
        gravity_debug_node(world, &kids[i], float3_add(position, (float3) { o.x * half, o.y * half, o.z * half }), half);
    }
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
        gravity_debug_node(world, field, position->value, scale->value * octree_size(depth->value));
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
