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
const color xr_dark_color = (color) { 0, 0, 0, 255 };
// - dark (huds3_dark, loading): black, only the 3D uis drawn (the "Loading" label), not the world as it loads

void xr_render_eye(ecs* world, float4x4 eye, float4 fov, float4x4 body, float2 viewport_position, float2 viewport_size, byte flip_y) {
    zox_gpu_set_viewport(viewport_position, viewport_size);
    const byte dark = huds3_dark;
    const color clear = dark ? xr_dark_color : xr_clear_color;
    zox_gpu_set_clear_color(color_to_float4(clear));
    zox_gpu_clear_rect(viewport_position, viewport_size);
    zox_gpu_enable_depth_test();
    zox_gpu_enable_culling();
    zox_gpu_disable_blend();
    renderer_camera = 0;
    render_camera_fov = 90;
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
    renderer_fog_color = color_to_color_rgb(clear);
    for (byte j = 0; j < max_render_loop_orders; j++) {
        for (size_t k = 0; k < render3D_systems->size; k++) {
            const entity system = render3D_systems->data[k];
            if (dark && system != zox_id(canvas3_render_system)) {
                continue;
            }
            const byte order = zox_has(system, RenderOrder) ? zox_getv(system, RenderOrder) : 0;
            if (order == j) {
                ecs_run(world, system, 0, NULL);
            }
        }
    }
}
