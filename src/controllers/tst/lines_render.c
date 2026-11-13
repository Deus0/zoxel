// tst: lines_render, 3D lines draw instanced, every line in one draw a camera, like particles
//  - in a game, spawns zox_tst_lines_render_count lines around the player's camera, waits a few frames
//  - this frame drew at least them (line3D_drawn), in a draw a camera, not a draw a line (line3D_draw_calls)
//  - saves a screenshot to tst/lines_render.ppm, to look at them
//  - logs the lines drawn and draws (builds with zox_disable_instancing draw a line at a time)
//  - run: bash bsh/tst.sh lines_render
byte zox_tst_player_spawn_ready(ecs* world);   // in _.c, after this is included
extern char zox_screenshot_request[256];
#define zox_tst_lines_render_count 2000
#define zox_tst_lines_render_frames 5
entity zox_tst_lines_render_lines[zox_tst_lines_render_count];
int zox_tst_lines_render_frame = 0;

byte zox_tst_lines_render_ready(ecs* world) {
    if (!zox_tst_lines_render_frame) {
        if (!zox_tst_player_spawn_ready(world)) {
            return 0;
        }
        const entity camera = zox_get_link(world, dbg_player, CameraLink);
        const float3 eye = zox_valid(camera) ? zox_getv(camera, Position3D) : float3_zero;
        const float4 rotation = zox_valid(camera) ? zox_getv(camera, Rotation3D) : quaternion_identity;
        // lines on a plane just in front of the camera
        const float3 forward = float4_rotate_float3(rotation, (float3) { 0, 0, 1 });
        const float3 right = float4_rotate_float3(rotation, (float3) { 1, 0, 0 });
        const float3 up = float4_rotate_float3(rotation, (float3) { 0, 1, 0 });
        // along the camera's ray through the middle of the screen, so it's in view
        const byte has_ray = zox_valid(camera) && zox_has(camera, RaycastOrigin) && zox_has(camera, RaycastNormal);
        const float3 ray_origin = has_ray ? zox_getv(camera, RaycastOrigin) : eye;
        const float3 ray_normal = has_ray ? zox_getv(camera, RaycastNormal) : forward;
        const float3 center = float3_add(ray_origin, float3_scale(ray_normal, 1.5f));
        // a fan of lines out from the center, easy to see in the screenshot
        for (int i = 0; i < zox_tst_lines_render_count; i++) {
            const float angle = (float) i / zox_tst_lines_render_count * 6.2831853f;
            const float3 b = float3_add(center, float3_add(
                float3_scale(right, cosf(angle) * 0.5f), float3_scale(up, sinf(angle) * 0.5f)));
            zox_tst_lines_render_lines[i] = spawn_line3c(world, center, b, 2, 0,
                (color_rgb) { (byte) (128 + 127 * cosf(angle)), (byte) (128 + 127 * sinf(angle)), 200 });
        }
    }
    zox_tst_lines_render_frame++;
    zox_tst_waiting = "the lines to render";
    if (zox_tst_lines_render_frame == zox_tst_lines_render_frames) {
        // a look at them: tst/lines_render.ppm
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("lines_render.ppm"));
    }
    return zox_tst_lines_render_frame > zox_tst_lines_render_frames && !zox_screenshot_request[0];
}

void zox_tst_lines_render_check(ecs* world) {
    zox_log("[tst] [metric] lines_render drew [%i] lines in [%i] draws this frame", line3D_drawn, line3D_draw_calls);
    zox_tst_check(line3D_drawn >= zox_tst_lines_render_count, "drew [%i] lines, spawned [%i]",
        line3D_drawn, zox_tst_lines_render_count);
#if !defined(zox_disable_instancing)
    // a draw a camera that renders lines, not one a line
    zox_tst_check(line3D_draw_calls > 0 && line3D_draw_calls <= 4, "[%i] draws for [%i] lines, expected instanced (a draw a camera)",
        line3D_draw_calls, line3D_drawn);
#endif
    for (int i = 0; i < zox_tst_lines_render_count; i++) {
        zox_delete(zox_tst_lines_render_lines[i]);
    }
}
