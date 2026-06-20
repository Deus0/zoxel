extern void spawn_line3t(ecs*, const float3, const float3, const color_rgb);

zox_sys2(ChunkBoundsDrawSystem) {
    const color_rgb up_color = (color_rgb) { 0, 0, 255 };
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Position3D);
    zox_sys_in(Bounds3D);
    zox_sys_in(RenderDisabled);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(RenderDisabled, renderDisabled);
        zox_sys_i(Position3D, position3D);
        zox_sys_i(Bounds3D, extents);

        if (renderDisabled->value) {
            continue;
        }

        const bounds chunk_bounds = {
            .center = float3_add(position3D->value, extents->value),
            .extents = extents->value
        };

        // const bounds chunk_bounds = calculate_chunk_bounds(position3D->value, chunkSize->value, blockScale->value);

        // zox_log("chunk_bounds %fx%fx%f - %fx%fx%f\n", chunk_bounds.center.x, chunk_bounds.center.y, chunk_bounds.center.z, chunk_bounds.extents.x, chunk_bounds.extents.y, chunk_bounds.extents.z)
        color_rgb line_color = up_color;
        //if (renderDisabled->value) line_color = up_color_disabled;
        spawn_cube_lines(world, chunk_bounds.center, chunk_bounds.extents, 1, line_color, 1);
        // spawn_line3t(world, chunk_bounds.center, float3_add(chunk_bounds.center, float3_up), line_color);
        spawn_line3t(world, float3_add(chunk_bounds.center, float3_down), float3_add(chunk_bounds.center, float3_up), line_color);
        spawn_line3t(world, float3_add(chunk_bounds.center, float3_left), float3_add(chunk_bounds.center, float3_right), line_color);

        spawn_line3t(world, float3_add(chunk_bounds.center, (float3) { 0, 0, -chunk_bounds.extents.z}), float3_add(chunk_bounds.center, (float3) { 0, 0, chunk_bounds.extents.z}), line_color);
        spawn_line3t(world, float3_add(chunk_bounds.center, (float3) { -chunk_bounds.extents.x, 0, 0 }), float3_add(chunk_bounds.center, (float3) { chunk_bounds.extents.x, 0, 0 }), line_color);

    }
} zox_sys_end(ChunkBoundsDrawSystem);
