void screen_render_texture_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_begin();
    zox_sys_in(LayoutSize);
    zox_sys_out(TextureSize);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(LayoutSize, lsize);
        zox_sys_o(TextureSize, tsize);
        tsize->value = scale_viewport(lsize->value);
        if (dbg_log) {
            zox_log("Render Texture Updated", tsize->value.x, tsize->value.y);
        }
    }
    zox_sys_on_end();
} zoxd_system(screen_render_texture_system);

static void layout_screen_render_texture(ecs* world, entity texture, entity canvas, entity camera) {
    const entity app = zox_get_link(world, canvas, AppLink);
    if (!zox_valid(app) || !zox_has(app, WindowSize) ||
        !zox_valid(camera) || !zox_has(camera, ScreenToCanvas))
    {
        return;
    }
    const int2 window_size = zox_getv(app, WindowSize);
    const float4 canvas_cell = zox_getv(canvas, ScreenToCanvas);
    const float4 camera_cell = zox_getv(camera, ScreenToCanvas);
    const int2 canvas_size = screen_to_canvas_size(window_size, canvas_cell);
    const int2 size = screen_to_canvas_size(window_size, camera_cell);
    const int2 origin = int2_sub(screen_to_canvas_position(window_size, camera_cell),
        screen_to_canvas_position(window_size, canvas_cell));
    const int2 position = int2_sub(int2_add(origin, int2_half(size)), int2_half(canvas_size));
    if (float4_equals(camera_cell, canvas_cell)) {
        if (!zox_has(texture, AnchorSize)) {
            zox_setv(texture, AnchorSize, float2_one);
        }
    } else {
        zox_remove(texture, AnchorSize);
    }
    if (!int2_equals(zox_getv(texture, LayoutSize), size) ||
        !int2_equals(zox_getv(texture, LayoutPosition), position))
    {
        zox_setv(texture, LayoutPosition, position);
        zox_setv(texture, LayoutSize, size);
        zox_add(texture, LayoutSizeDirty);
    }
}

static void screen_render_texture_camera_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        const entity camera = it->entities[i];
        const entity texture = zox_get_link(world, camera, RenderTextureLink);
        if (zox_valid(texture) && zox_has(texture, RenderTextureScreen)) {
            layout_screen_render_texture(world, texture, zox_get_parent(world, texture), camera);
        }
    }
}

static void screen_render_texture_canvas_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        const entity canvas = it->entities[i];
        const entity texture = zox_get_child_by_id(world, canvas, zox_id(RenderTextureScreen));
        if (texture) {
            layout_screen_render_texture(world, texture, canvas, zox_get_link(world, texture, CameraLink));
        }
    }
}
