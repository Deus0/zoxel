// hdr output (doc/plans/hdr.md B, graphics/dat/hdr_output.c): the screen shows past sdr white
//  - while on, the frame draws into a half float canvas the window's size (its render textures' passes go back to it,
//    zox_gpu_default_fbo), then one draw encodes it into the swapchain: hdr10 (pq, rec.2020) or scrgb (linear)
//  - its colours are as the game's always are (display values, the ui's up to 1, the 3D view's past it with the
//    tonemap's peak): made linear, sdr white at zox_hdr_paper_white nits
//  - zox_hdr_output_canvas: the same canvas and draw over an 8 bit swapchain, passed through (looks as off, a test)
float zox_hdr_paper_white = 203;    // nits sdr white (the ui, a tonemap's 1) shows at, bt.2408's
guint zox_hdr_canvas_texture = 0;
guint zox_hdr_canvas_fbo = 0;
guint zox_hdr_canvas_rbo = 0;
int2 zox_hdr_canvas_size = { 0, 0 };
guint zox_hdr_encode_program = 0;

static const char* zox_hdr_encode_vertex_source =
"out vec2 uv;\n"
"void main() {\n"
"    vec2 corner = vec2(float((gl_VertexID << 1) & 2), float(gl_VertexID & 2));\n"   // a triangle over the screen
"    uv = corner;\n"
"    gl_Position = vec4(corner * 2.0 - 1.0, 0.0, 1.0);\n"
"}\n";

static const char* zox_hdr_encode_fragment_source =
"uniform sampler2D canvas;\n"
"uniform vec4 encode;\n"                // x: 1 hdr10, 2 scrgb, else passed through; y: paper white nits; z: 1 flip v
"in vec2 uv;\n"
"out vec4 frag_color;\n"
"vec3 linear_from_display(vec3 c) {\n"    // srgb's curve, past 1 carried on
"    vec3 low = c / 12.92;\n"
"    vec3 high = pow((max(c, vec3(0.0)) + 0.055) / 1.055, vec3(2.4));\n"
"    return mix(high, low, vec3(lessThanEqual(c, vec3(0.04045))));\n"
"}\n"
"vec3 pq(vec3 nits) {\n"                  // st2084, nits over 10000
"    vec3 y = pow(clamp(nits / 10000.0, 0.0, 1.0), vec3(0.1593017578125));\n"
"    return pow((0.8359375 + 18.8515625 * y) / (1.0 + 18.6875 * y), vec3(78.84375));\n"
"}\n"
"void main() {\n"
"    vec2 at = encode.z > 0.5 ? vec2(uv.x, 1.0 - uv.y) : uv;\n"
"    vec3 c = texture(canvas, at).rgb;\n"
"    if (encode.x > 0.5 && encode.x < 1.5) {\n"
"        vec3 nits = linear_from_display(c) * encode.y;\n"
"        const mat3 bt709_to_bt2020 = mat3(0.6274, 0.0691, 0.0164, 0.3293, 0.9195, 0.0880, 0.0433, 0.0114, 0.8956);\n"
"        c = pq(max(bt709_to_bt2020 * nits, vec3(0.0)));\n"
"    } else if (encode.x > 1.5) {\n"
"        c = linear_from_display(c) * encode.y / 80.0;\n"
"    }\n"
"    frag_color = vec4(c, 1.0);\n"
"}\n";

static inline void hdr_output_dispose_canvas() {
    if (zox_hdr_canvas_fbo) {
        zox_gpu_dispose_fbo(zox_hdr_canvas_fbo);
    }
    if (zox_hdr_canvas_rbo) {
        zox_gpu_dispose_rbo(zox_hdr_canvas_rbo);
    }
    if (zox_hdr_canvas_texture) {
        zox_gpu_dispose_texture(zox_hdr_canvas_texture);
    }
    zox_hdr_canvas_texture = zox_hdr_canvas_fbo = zox_hdr_canvas_rbo = 0;
    zox_hdr_canvas_size = int2_zero;
    zox_gpu_default_fbo = 0;
}

// the canvas the window's size, made or made again on a resize; 0 if it can't be
static inline byte hdr_output_canvas(int2 size) {
    if (zox_hdr_canvas_fbo && int2_equals(size, zox_hdr_canvas_size)) {
        return 1;
    }
    hdr_output_dispose_canvas();
    if (!zox_hdr_encode_program) {
        zox_hdr_encode_program = zox_gpu_draw_program(zox_hdr_encode_vertex_source, zox_hdr_encode_fragment_source);
    }
    if (!zox_hdr_encode_program) {
        return 0;
    }
    zox_hdr_canvas_texture = spawn_gpu_texture_buffer();
    zox_gpu_set_texture_color_hdr(zox_hdr_canvas_texture, size, 0);
    zox_hdr_canvas_rbo = zox_gpu_create_rbo();
    zox_gpu_set_rbo_size(zox_hdr_canvas_rbo, size);
    zox_hdr_canvas_fbo = zox_gpu_create_fbo();
    zox_gpu_link_fbo_rbo(zox_hdr_canvas_fbo, zox_hdr_canvas_rbo);
    zox_gpu_fbo_to_texture(zox_hdr_canvas_fbo, zox_hdr_canvas_texture);
    zox_hdr_canvas_size = size;
    return 1;
}

// before the cameras draw: the frame into the canvas (its render textures' passes back to it after)
byte zox_hdr_output_was = zox_hdr_output_off;

// the HDR setting greyed while its screen isn't in hdr (zox_gpu_hdr_output_supported), as the window moves screens or
//  hdr's turned on or off: off while greyed, its value back when it isn't (reopen options to see it change)
static inline void hdr_output_sync_setting(ecs* world) {
    const entity setting = zox_valid(main_app) ? zox_get_child_by_id(world, main_app, zox_id(SettingHdr)) : 0;
    if (!zox_valid(setting) || zox_hdr_output_asked == zox_hdr_output_canvas) {
        return;
    }
    const byte supported = zox_gpu_hdr_output_supported() && !zox_xr_active;
    if (supported == !zox_has(setting, SettingUnsupported)) {
        return;
    }
    if (supported) {
        zox_remove(setting, SettingUnsupported);
    } else {
        zox_add(setting, SettingUnsupported);
    }
    zox_gpu_set_hdr_output(supported && zox_getv(setting, SettingByte) ? zox_hdr_output_hdr10 : zox_hdr_output_off);
}

// once a frame, before the first camera or canvas draws
void hdr_output_begin(ecs* world) {
    static uint done_frame = UINT32_MAX;
    if (done_frame == ecs_run_count) {
        return;
    }
    done_frame = ecs_run_count;
    hdr_output_sync_setting(world);
    // the swapchain's hdr changed (the setting, a screen): render cameras' targets to half floats or back (fun/hdr.c)
    if (zox_hdr_output != zox_hdr_output_was) {
        zox_hdr_output_was = zox_hdr_output;
        zox_remake_render_textures(world);
    }
    const byte on = zox_hdr_output != zox_hdr_output_off && zox_valid(main_app) && zox_has(main_app, WindowSize);
    if (!on) {
        if (zox_hdr_canvas_fbo) {
            hdr_output_dispose_canvas();
        }
        return;
    }
    const int2 size = zox_getv(main_app, WindowSize);
    if (size.x <= 0 || size.y <= 0 || !hdr_output_canvas(size)) {
        return;
    }
    zox_gpu_default_fbo = zox_hdr_canvas_fbo;
    zox_gpu_bind_fbo(zox_hdr_canvas_fbo);
    zox_gpu_set_viewport(float2_zero, int2_to_float2(size));
    zox_gpu_set_clear_color(color_to_float4(viewport_clear_color));
    zox_gpu_clear_viewport();
}

// after everything's drawn, before the screenshot and present: the canvas encoded into the swapchain
void hdr_output_end(ecs* world) {
    (void) world;
    if (!zox_hdr_canvas_fbo || !zox_gpu_default_fbo) {
        return;
    }
    zox_gpu_default_fbo = 0;
    zox_gpu_reset_fbo();
    zox_gpu_set_viewport(float2_zero, int2_to_float2(zox_hdr_canvas_size));
    zox_gpu_material(zox_hdr_encode_program);
    zox_gpu_bind_texture(zox_hdr_canvas_texture);
    const float mode = zox_hdr_output == zox_hdr_output_hdr10 ? 1 : zox_hdr_output == zox_hdr_output_scrgb ? 2 : 0;
    zox_gpu_program_float4(zox_hdr_encode_program, "encode", (float4) { mode, zox_hdr_paper_white, 0, 0 });
    zox_gpu_render_triangles(3);
    zox_disable_material();
}

void gpu_dispose_hdr_output(ecs* world) {
    (void) world;
    hdr_output_dispose_canvas();
    if (zox_hdr_encode_program) {
        zox_gpu_dispose_program(zox_hdr_encode_program);
        zox_hdr_encode_program = 0;
    }
}

void gpu_restore_hdr_output(ecs* world) {
    (void) world;
    zox_hdr_canvas_texture = zox_hdr_canvas_fbo = zox_hdr_canvas_rbo = 0;   // made again at the next frame
    zox_hdr_canvas_size = int2_zero;
    zox_hdr_encode_program = 0;
}
