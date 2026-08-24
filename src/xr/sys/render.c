static inline float4x4 xr_calculate_ui_projection_matrix(
    int2 screen_size)
{
    const float aspect_ratio =
        (float) screen_size.x / (float) screen_size.y;

    // Make the existing UI world 10x smaller on screen.
    const float half_height = 5.0f;
    const float half_width = half_height * aspect_ratio;

    const float left = -half_width;
    const float right = half_width;
    const float top = -half_height;
    const float bottom = half_height;

    const float znear = -1.0f;
    const float zfar = 1.0f;

    float4x4 matrix = float4x4_zero;

    matrix.x.x = 2.0f / (right - left);
    matrix.y.y = 2.0f / (top - bottom);
    matrix.z.z = -2.0f / (zfar - znear);

    matrix.w.x = -(right + left) / (right - left);
    matrix.w.y = -(top + bottom) / (top - bottom);
    matrix.w.z = -(zfar + znear) / (zfar - znear);
    matrix.w.w = 1.0f;

    return matrix;
}

// NOTE: NO need to time as it runs systems inside it
void xr_camera_render_system(iter* it) {
    byte dbg_log = 0;
    zox_sys_world();
    // XR Triangle direction
    glFrontFace(GL_CW);
    // NOTE: This one is regular one
    // glFrontFace(GL_CCW);
    color clear_color = (color) { 33, 38, 38, 255 };
    float4x4 world_camera = float4x4_identity;
    if (zox_valid(xr_camera)) {
        world_camera = zox_getv(xr_camera, TransformMatrix);
    }
    float near_distance = 0.01f;
    float far_distance = 800;
    byte running = 1;
    if (!xr_can_render(&running)) {
        if (!running) {
            if (dbg_log) {
                zox_log("Running XR stopped, ending Game");
            }
            engine_end();
        } else {
            if (dbg_log) {
                zox_log("XR not rendering this frame");
            }
        }
        return;
    }
    xr_get_eyes();
    for (int i = 0; i < 2; i++) {
        xr_eye eye = xr_eyes[i];
        // our actual render code here
        if (!xr_eye_render_begin(eye.swapchain, eye.images, eye.fbo)) {
            zox_loge("[XR] xr_eye_render_begin failed");
            continue;
        }
        zox_gpu_set_viewport(
            float2_zero,
            int2_to_float2(eye.image_size)
        );
        zox_gpu_set_clear_color(color_to_float4(clear_color));
        zox_gpu_clear_viewport();
        // xr_test_render_cubes(vp_matrix);
        // --------------------
        // 3D rendering
        // --------------------
        zox_gpu_enable_depth_test();        // cull for 3D things
        zox_gpu_enable_culling();
        zox_gpu_disable_blend();
        renderer_camera = 0;
        render_camera_fov = 90;
        // calculate our vp
        float4x4 world_projection = xr_create_projection_matrix(
            eye.fov,
            near_distance,
            far_distance
        );
        float4x4 world_eye = matrix_multiply(world_camera, eye.matrix);
        render_camera_position = float4x4_get_position(world_eye);
        float4x4 view_matrix = float4x4_inverse(world_eye);
        render_camera_matrix = matrix_multiply(world_projection, view_matrix);
        // render_camera_matrix = float4x4_multiply(world_projection, view_matrix);
        renderer_fog_color = color_to_color_rgb(clear_color);
        for (byte j = 0; j < max_render_loop_orders; j++) {
            for (size_t k = 0; k < render3D_systems->size; k++) {
                entity system = render3D_systems->data[k];
                byte order = zox_has(system, RenderOrder) ? zox_getv(system, RenderOrder) : 0;
                if (order == j) {
                    if (dbg_log >= 2) {
                        zox_log(" - %i [%s] (%i)", k, zox_getn(system), j);
                    }
                    ecs_run(world, system, 0, NULL);
                }
            }
        }
        // --------------------
        // 2D rendering
        // --------------------
        /*zox_gpu_disable_depth_test();
        zox_gpu_disable_culling();
        zox_gpu_disable_blend();
        // Switch camera to 2D orthographic projection
        float4x4 ui_projection = xr_calculate_ui_projection_matrix(eye.image_size);
        float4x4 ui_transform = float4x4_identity;
        // ui_transform.w.z = -2.0f;
        // float4x4 ui_view = float4x4_inverse(eye.matrix);
        // render_camera_matrix = ui_projection;
        // render_camera_matrix = matrix_multiply(ui_projection, view_matrix);
        render_camera_matrix =
            matrix_multiply(
                ui_projection,
                matrix_multiply(view_matrix, ui_transform)
            );
        render_camera_position = float3_zero;
        render_camera_fov = 0;
        renderer_camera = 0;
        clear_ui_layers();
        for (size_t j = 0; j < render2D_systems->size; j++) {
            entity system = render2D_systems->data[j];
            ecs_run(world, system, 0, NULL);
        }
        render_uis(world, zox_ui_render_queue);*/
        // Finish Rendering
        xr_eye_render_end(eye.swapchain);
    }
    xr_end_frame();
} zoxd_system(xr_camera_render_system);

