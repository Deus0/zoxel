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
//  and a float4x4 only so it lays out as in c), uploaded once a camera (render_view_upload), read by every draw
//  (doc/howto/shaders.md)
typedef struct {
    float4 fog_data;        // the fog's colour (xyz, darkened by the sky's light) and density (w)
    float4 sky_tint;        // the sky's light on the world (xyz): its suns summed, at least the night's
    float4 sun_direction;   // towards the brightest sun (xyz), normalized
    float4 sun_color;       // its colour (xyz): its realm's sunset low down
    float4 camera_position; // the camera's place (xyz): height fog
    float4 frag_viewport;   // a fragment's gl_FragCoord back to the viewport (zox_gpu_frag_viewport)
    float4x4 clip_to_view;  // clip space back to a point's offset from the camera, world axes (fog by direction)
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

// a camera starting to draw, its render_ values, viewport and target set: its view for every shader, its sky from
//  the camera
void render_view_upload(ecs* world, entity camera, float2 viewport_position, float2 viewport_size) {
    if (!render_view_ubo) {
        return;
    }
    const float3 tint = zox_valid(camera) && zox_has(camera, SkyTint) ? zox_getv(camera, SkyTint) : float3_one;
    const float3 sun = zox_valid(camera) && zox_has(camera, SunDirection) ? zox_getv(camera, SunDirection) : float3_up;
    const float3 sun_color = zox_valid(camera) && zox_has(camera, SunColor) ? zox_getv(camera, SunColor) : float3_one;
    float4 fog = get_fog_value();
    fog.x *= tint.x;
    fog.y *= tint.y;
    fog.z *= tint.z;
    // the view projection with the camera at the origin, inverted: offsets stay small (mediump in the shaders)
    const float4x4 relative = float4x4_multiply(float4x4_position(render_camera_position), render_camera_matrix);
    const RenderView view = {
        fog,
        float4_from_float3(tint, 1),
        float4_from_float3(sun, 0),
        float4_from_float3(sun_color, 1),
        float4_from_float3(render_camera_position, 1),
        zox_gpu_frag_viewport(viewport_position, viewport_size),
        float4x4_inverse(relative)
    };
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