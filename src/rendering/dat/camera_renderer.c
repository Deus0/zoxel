// stores camera data to use in camera pass systems
entity renderer_camera;
float4x4 render_camera_matrix;
float3 render_camera_position;
color_rgb renderer_fog_color;
byte renderer_layer = 0;
byte render_camera_fov;
float render_viewport_height = 1;    // the drawn view's height in pixels (points sized in the world: particles)

// fog
byte is_render_fog = 1;
float fog_density = 0.03f; // 0326f;
float fog_settings_mul = 0.048f;   // the Fog setting (0 to 1) to fog_density

static inline float get_fog_density() {
    return is_render_fog ?
        fog_density * fog_density :
        0;
}

// the camera drawing now is under water (cameras.Underwater, set in camera_render3_system): a thick blue fog
byte render_underwater = 0;
const color_rgb render_underwater_color = { 28, 72, 120 };
#define render_underwater_density 0.15f   // fully blue about 5m out, 9 blocks (fog is 1.1 - 2^(-density * z^2))

static inline float4 get_fog_value() {
    const color_rgb color = render_underwater ? render_underwater_color : renderer_fog_color;
    const float density = render_underwater ? render_underwater_density : get_fog_density();
    return float4_from_float3(color_rgb_to_float3(color), density);
}

// what every 3D shader knows of the camera drawing now: res/shaders/view.glsl's RenderView block (std140, float4s
//  only so it lays out as in c), uploaded once a camera (render_view_upload), read by every draw (doc/howto/shaders.md)
typedef struct {
    float4 fog_data;    // the fog's colour (xyz) and density (w)
} RenderView;
#define zox_render_view_binding 1
guint render_view_ubo = 0;

// made before any material (process_shaders), again after a gpu restore
void render_view_create() {
    render_view_ubo = zox_gpu_create_buffer();
    zox_gpu_ubo_initialize(render_view_ubo, zox_render_view_binding, sizeof(RenderView));
    zox_gpu_ubo_reset();
}

void render_view_dispose() {
    if (render_view_ubo) {
        zox_gpu_dispose_buffer(render_view_ubo);
        render_view_ubo = 0;
    }
}

// a camera starting to draw, its render_ values set: its view for every shader
void render_view_upload() {
    if (!render_view_ubo) {
        return;
    }
    const RenderView view = { get_fog_value() };
    zox_gpu_ubo_set(render_view_ubo, sizeof(RenderView), &view);
    zox_gpu_ubo_reset();
}

void set_camera_fog_color(ecs* world, color_rgb fog) {
    for (int i = 0; i < main_cameras_count; i++) {
        entity camera = main_cameras[i];
        if (zox_valid(camera)) {
            zox_setv(camera, FogColor, fog);
        } else {
            zox_logw("Main Camera Invalid at [%i]", i)
        }
    }
}