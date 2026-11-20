// tst: lines_render, 3D lines draw instanced, every line in one draw a camera, like particles
//  - on the menus (no realm or player, a camera is all it needs): spawns zox_tst_lines_render_count lines in front of
//    the main camera in one zox_bulk_spawn (it tests drawing them, not spawning), waits a few frames
//  - this frame drew at least them (line3D_drawn), in a draw a camera, not a draw a line (line3D_draw_calls)
//  - saves a screenshot to tst/lines_render.ppm, to look at them
//  - logs the lines drawn and draws (builds with zox_disable_instancing draw a line at a time)
//  - run: bash bsh/tst.sh lines_render
extern char zox_screenshot_request[256];
#define zox_tst_lines_render_count 2000
#define zox_tst_lines_render_frames 5
int zox_tst_lines_render_frame = 0;

// a fan of lines out from a point just in front of the camera, easy to see in the screenshot
void zox_tst_lines_render_spawn(ecs* world, entity camera) {
    const float3 eye = zox_getv(camera, Position3D);
    const float4 rotation = zox_getv(camera, Rotation3D);
    const float3 right = float4_rotate_float3(rotation, (float3) { 1, 0, 0 });
    const float3 up = float4_rotate_float3(rotation, (float3) { 0, 1, 0 });
    // a camera looks down its -z (cameras/fun/free_roam.c)
    const float3 forward = float4_rotate_float3(rotation, (float3) { 0, 0, -1 });
    const float3 center = float3_add(eye, float3_scale(forward, 1.5f));
    static LineData3D lines[zox_tst_lines_render_count];
    static LineThickness thicknesses[zox_tst_lines_render_count];
    static Color colors[zox_tst_lines_render_count];
    for (int i = 0; i < zox_tst_lines_render_count; i++) {
        const float angle = (float) i / zox_tst_lines_render_count * 6.2831853f;
        const float3 b = float3_add(center, float3_add(
            float3_scale(right, cosf(angle) * 0.5f), float3_scale(up, sinf(angle) * 0.5f)));
        lines[i].value = (float6) { center.x, center.y, center.z, b.x, b.y, b.z };
        thicknesses[i].value = 2;
        colors[i].value = color_rgb_to_color((color_rgb) {
            (byte) (128 + 127 * cosf(angle)), (byte) (128 + 127 * sinf(angle)), 200 });
    }
    const ecs_id_t ids[] = { zox_isa_id(prefab_line3D), zox_id(LineData3D), zox_id(LineThickness), zox_id(Color) };
    void* data[] = { NULL, lines, thicknesses, colors };
    zox_bulk_spawn(world, zox_tst_lines_render_count, ids, data, 4);
}

byte zox_tst_lines_render_ready(ecs* world) {
    if (!zox_tst_lines_render_frame) {
        const entity camera = main_cameras[0];
        if (!zox_valid(camera)) {
            zox_tst_waiting = "the menu's camera";
            return 0;
        }
        zox_tst_lines_render_spawn(world, camera);
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
    (void) world;
    zox_log("[tst] [metric] lines_render drew [%i] lines in [%i] draws this frame", line3D_drawn, line3D_draw_calls);
    zox_tst_check(line3D_drawn >= zox_tst_lines_render_count, "drew [%i] lines, spawned [%i]",
        line3D_drawn, zox_tst_lines_render_count);
#if !defined(zox_disable_instancing)
    // a draw a camera that renders lines, not one a line
    zox_tst_check(line3D_draw_calls > 0 && line3D_draw_calls <= 4, "[%i] draws for [%i] lines, expected instanced (a draw a camera)",
        line3D_draw_calls, line3D_drawn);
#endif
}

void zox_tsts_lines_render(ecs* world) {
    zox_tst_add("lines_render", zox_tst_lines_render_ready, zox_tst_lines_render_check, 30);
    zox_tst_set_setup("lines_render", NULL, 1);
}
