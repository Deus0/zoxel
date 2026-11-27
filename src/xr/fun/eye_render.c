// One eye's 3D render, the same for a headset and the simulator
//  - eye: the eye in the head's space (a headset's located pose, or the simulator's), fov: its four angles
//  - body: the roam's transform, the eye on top of it (eye, then body: tst camera_xr_eyes)
//  - draws into whatever is bound (a swapchain image, or the window) at the viewport given
//  - y up, triangles counter clockwise, as every other camera (tst xr_sim_eyes: the ground under the horizon)
//  - flip_y: a headset's swapchain images are read top down, y flipped (its triangles clockwise, openxr/render.c);
//      without it the headset showed the world upside down, its turning and the eyes' skew opposite
#define xr_near_distance 0.01f
#define xr_far_distance 800.0f
const color xr_clear_color = (color) { 33, 38, 38, 255 };
// - the eyes closing (its camera's CameraLids: loading, sleeping): dark lids from the top and the bottom over the world, their edges
//   curved (the corners shut first, an eye's almond opening: xr_eye_lids); shut, only the 3D uis are drawn (the "Loading" label, fading in:
//   huds3_label_fade), not the world as it loads; the 3D uis drawn over the lids
//   - the fog fading the world to black didn't blacken a headset's view (play test)
#define xr_lid_columns 96               // the lids' edges, in this many steps across an eye
#define xr_lid_top 0.6f                 // of the eye's height, the top lid's share closing (the bottom's the rest)
#define xr_lid_curve 0.8f               // the lids' edges curved: the corners this much further in

// how far a lid comes in at u (-1 to 1 across the eye), of the eye's height, closed by closure (0 to 1)
static inline float xr_lid_depth(float share, float closure, float u) {
    return float_min(1, share * closure * (1 + xr_lid_curve * u * u));
}

// the eye's lids, black, closed by closure: columns cleared from each edge in to the lid's curve
//  - flip_y: a headset's images are read top down, the top lid at the bottom of the buffer
static void xr_eye_lids(float2 position, float2 size, float closure, byte flip_y) {
    if (closure <= 0) {
        return;
    }
    zox_gpu_set_clear_color(float4_from_float3((float3) { 0, 0, 0 }, 1));
    for (int i = 0; i < xr_lid_columns; i++) {
        const float x0 = floorf(position.x + size.x * i / xr_lid_columns);
        const float x1 = floorf(position.x + size.x * (i + 1) / xr_lid_columns);
        const float u = ((i + 0.5f) / xr_lid_columns) * 2 - 1;
        const float top = ceilf(size.y * xr_lid_depth(xr_lid_top, closure, u));
        const float bottom = ceilf(size.y * xr_lid_depth(1 - xr_lid_top, closure, u));
        const float upper = flip_y ? bottom : top;      // the buffer's top (y up)
        const float lower = flip_y ? top : bottom;
        if (upper > 0) {
            zox_gpu_clear_rect((float2) { x0, position.y + size.y - upper }, (float2) { x1 - x0, upper });
        }
        if (lower > 0) {
            zox_gpu_clear_rect((float2) { x0, position.y }, (float2) { x1 - x0, lower });
        }
    }
}

void xr_render_eye(ecs* world, float4x4 eye, float4 fov, float4x4 body, float2 viewport_position, float2 viewport_size, byte flip_y) {
    zox_gpu_set_viewport(viewport_position, viewport_size);
    // its camera's lids (CameraLids: loading, sleeping)
    const float closure = zox_valid(xr_camera) && zox_has(xr_camera, CameraLids) ? zox_getv(xr_camera, CameraLids) : 0;
    const byte shut = closure >= 1;
    const color clear = shut ? (color) { 0, 0, 0, 255 } : xr_clear_color;
    zox_gpu_set_clear_color(color_to_float4(clear));
    zox_gpu_clear_rect(viewport_position, viewport_size);
    zox_gpu_enable_depth_test();
    zox_gpu_enable_culling();
    zox_gpu_disable_blend();
    renderer_camera = 0;
    render_camera_fov = 90;
    render_viewport_height = viewport_size.y;
    float4x4 projection = camera_projection_from_angles(fov, xr_near_distance, xr_far_distance);
    if (flip_y) {
        projection.x.y = -projection.x.y;
        projection.y.y = -projection.y.y;
        projection.z.y = -projection.z.y;
        projection.w.y = -projection.w.y;
    }
    // the eye's tracked place scaled to the body (fun/scale.c): its height, its distance from the other eye
    eye.w.x *= xr_view_scale;
    eye.w.y *= xr_view_scale;
    eye.w.z *= xr_view_scale;
    const float4x4 world_eye = float4x4_multiply(eye, body);    // the eye, then the body
    render_camera_position = float4x4_get_position(world_eye);
    render_camera_matrix = float4x4_multiply(float4x4_inverse(world_eye), projection);   // the view, then the projection
    // the camera's fog, the sky's colour (the clear colour's grey didn't blend the terrain into the sky)
    renderer_fog_color = zox_valid(xr_camera) && zox_has(xr_camera, FogColor) ? zox_getv(xr_camera, FogColor) : color_to_color_rgb(xr_clear_color);
    render_underwater = terrain_water_at(world, local_terrain, render_camera_position);   // src/terrains/voxel/sys/water_view.c
    render_view_upload(world, xr_camera, viewport_position, viewport_size);
    // the world (shut, none of it), then the lids over it, then the 3D uis over them
    for (byte ui = 0; ui < 2; ui++) {
        if (ui) {
            xr_eye_lids(viewport_position, viewport_size, closure, flip_y);
            element3_brightness = shut ? huds3_label_fade : 1;
        } else if (shut) {
            continue;
        }
        for (byte j = 0; j < max_render_loop_orders; j++) {
            for (size_t k = 0; k < render3D_systems->size; k++) {
                const entity system = render3D_systems->data[k];
                if ((system == zox_id(canvas3_render_system)) != ui) {
                    continue;
                }
                const byte order = zox_has(system, RenderOrder) ? zox_getv(system, RenderOrder) : 0;
                if (order == j) {
                    ecs_run(world, system, 0, NULL);
                }
            }
        }
    }
    element3_brightness = 1;
    render_underwater = 0;
}
