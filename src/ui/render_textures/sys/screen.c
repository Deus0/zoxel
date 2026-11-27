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

// where a camera's screen texture goes on its canvas: its cell's size, its middle from the canvas's; 0 if it can't
//  tell (no window, no cell), 2 if its cell is the canvas's (it fills it)
static byte screen_render_texture_cell(ecs* world, entity canvas, entity camera, int2* position, int2* size) {
    const entity app = zox_get_link(world, canvas, AppLink);
    if (!zox_valid(app) || !zox_has(app, WindowSize) ||
        !zox_valid(camera) || !zox_has(camera, ScreenToCanvas))
    {
        return 0;
    }
    const int2 window_size = zox_getv(app, WindowSize);
    const float4 canvas_cell = zox_getv(canvas, ScreenToCanvas);
    const float4 camera_cell = zox_getv(camera, ScreenToCanvas);
    const int2 canvas_size = screen_to_canvas_size(window_size, canvas_cell);
    *size = screen_to_canvas_size(window_size, camera_cell);
    const int2 origin = int2_sub(screen_to_canvas_position(window_size, camera_cell),
        screen_to_canvas_position(window_size, canvas_cell));
    *position = int2_sub(int2_add(origin, int2_half(*size)), int2_half(canvas_size));
    return float4_equals(camera_cell, canvas_cell) ? 2 : 1;
}

static void layout_screen_render_texture(ecs* world, entity texture, entity canvas, entity camera) {
    int2 position, size;
    const byte cell = screen_render_texture_cell(world, canvas, camera, &position, &size);
    if (!cell) {
        return;
    }
    if (cell == 2) {
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

// a viewport camera's screen texture (CameraViewport), over the game view on its canvas: its own material (its pass
//  sets its camera's effects), held by its camera (UIHolderLink: deleted with it); 0 without a game view on a canvas
//  (no screen, xr): then it draws straight to its part of the screen, not a render camera
static entity spawn_viewport_render_texture(ecs* world, entity camera) {
    const entity game_view = zox_valid(main_cameras[0]) ? zox_get_link(world, main_cameras[0], RenderTextureLink) : 0;
    const entity canvas = zox_valid(game_view) ? zox_get_parent(world, game_view) : 0;
    int2 position, size;
    const byte cell = zox_valid(canvas) ? screen_render_texture_cell(world, canvas, camera, &position, &size) : 0;
    if (zox_xr_active || !cell) {
        zox_remove(camera, RenderCamera);
        return 0;
    }
    const entity shader = zox_get_link(world, material_render_texture_rgb, ShaderLink);
    const entity material = spawn_material_render_texture_from(world, shader, zox_getv(shader, ShaderGPULink));
    // laid out as it's made, in its cell (layout_screen_render_texture once the cell or canvas changes)
    const entity texture = spawn_render_texture(world, prefab_render_texture_screen, canvas, float2_centre, position,
        size, scale_viewport(size), 1, camera, material);
    if (cell != 2) {
        zox_remove(texture, AnchorSize);
    }
    zox_add(texture, TextureRGB);
    zox_link(world, texture, UIHolderLink, camera);
    zox_set_parent(world, material, texture);
    return texture;
}

static void screen_render_texture_camera_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        const entity camera = it->entities[i];
        const entity texture = zox_get_link(world, camera, RenderTextureLink);
        if (!zox_valid(texture) && zox_has(camera, CameraViewport) && zox_has(camera, RenderCamera)) {
            spawn_viewport_render_texture(world, camera);
        } else if (zox_valid(texture) && zox_has(texture, RenderTextureScreen)) {
            layout_screen_render_texture(world, texture, zox_get_parent(world, texture), camera);
        }
    }
}

// each of its screen textures: the game view's and its viewport cameras'
static void screen_render_texture_canvas_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        const entity canvas = it->entities[i];
        iter it2 = zox_children(world, canvas);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                const entity texture = it2.entities[j];
                if (zox_has(texture, RenderTextureScreen)) {
                    layout_screen_render_texture(world, texture, canvas, zox_get_link(world, texture, CameraLink));
                }
            }
        }
    }
}
