// NOTE: NO need to time as it runs systems inside it
void xr_camera_render_system(iter* it) {
    byte dbg_log = 1;
    zox_sys_world();
    zox_gpu_enable_depth_test();        // cull for 3D things
    zox_gpu_enable_culling();
    zox_gpu_disable_blend();
    // XR Triangle direction
    glFrontFace(GL_CW);
    // NOTE: This one is regular one
    // glFrontFace(GL_CCW);
    color clear_color = (color) { 33, 38, 38, 255 };
    float4x4 world_camera = float4x4_identity;
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
        float4x4 world_eye =
            matrix_multiply(world_camera, eye.matrix);
        float4x4 view_matrix =
            float4x4_inverse(world_eye);
        // calculate our vp
        float4x4 projection = xr_create_projection_matrix(
            eye.fov, near_distance, far_distance);
        float4x4 vp_matrix = matrix_multiply(projection, view_matrix);
        // our actual render code here
        if (!xr_eye_render_begin(eye.swapchain, eye.images, eye.fbo)) {
            zox_loge("[XR] xr_eye_render_begin failed");
            continue;
        }
        {
            zox_gpu_set_viewport(
                float2_zero,
                int2_to_float2(eye.image_size));
            zox_gpu_set_clear_color(color_to_float4(clear_color));
            zox_gpu_clear_viewport();
            // xr_test_render_cubes(vp_matrix);
            renderer_camera = 0;
            render_camera_fov = 110;
            render_camera_matrix = vp_matrix;
            render_camera_position = float4x4_get_position(render_camera_matrix);
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
        }
        xr_eye_render_end(eye.swapchain);
    }
    xr_end_frame();
} zoxd_system(xr_camera_render_system);

